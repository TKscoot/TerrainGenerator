#include "Terrain.h"


void CTerrain::Initialize(PSSMShadowCameraSetup* pssmSetup)
{
	mPssmSetup = pssmSetup;

	mTerrainGlobals = new Ogre::TerrainGlobalOptions();
	mTerrainGroup = new Ogre::TerrainGroup(mSceneManager, Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
	mTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
	mTerrainGroup->setOrigin(mTerrainPos);

	Ogre::Light* l = mSceneManager->createLight("terrainLight");
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDiffuseColour(ColourValue::White);
	l->setSpecularColour(ColourValue(0.4, 0.4, 0.4));
	Ogre::SceneNode* ln = mSceneManager->getRootSceneNode()->createChildSceneNode();
	ln->setDirection(Vector3(0.55, -0.8, 0.75).normalisedCopy());
	ln->attachObject(l);

	ConfigureTerrainDefaults(/*l*/);

	CreateTerrain();

	CEvent::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CTerrain::Update, this, std::placeholders::_1));
}

bool CTerrain::Update(const FrameEvent& evt)
{

	std::ostringstream ss;
	const char* str0 = mSeed.c_str();
	char* str = const_cast<char*>(str0);
	ImGui::Begin("Terrain");
	if (ImGui::InputText("Seed:", str, IM_ARRAYSIZE(str)))
	{
		ss << str0;
		mSeed = ss.str();
	}
	ImGui::SliderInt("Octaves:", &mOctaves, 1, 15);
	ImGui::SliderFloat("Frequency:", &mFrequency, 0.05f, 5.0f);
	ImGui::SliderFloat("Height Scale:", &mHeightScale, 0.05f, 2.5f);
	ImGui::SliderFloat("Power factor:", &mPowerFactor, 0.1f, 3.0f);
	ImGui::Spacing();
	if (ImGui::Button("Generate!"))
	{
		//CreateTerrain();
		UpdateTerrainHeight(0, 0);
	}
	ImGui::End();
	return true;
}

void CTerrain::ConfigureTerrainDefaults(/*Light * l*/)
{
	// Configure global
	mTerrainGlobals->setMaxPixelError(8);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(30000);

	Light* l = mSceneManager->getLight("terrainLight");

	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mTerrainGlobals->setUseVertexCompressionWhenAvailable(false);
	mTerrainGlobals->setCastsDynamicShadows(true);
	mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(mSceneManager->getAmbientLight());
	mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());
	mTerrainGlobals->setUseRayBoxDistanceCalculation(true);

	Ogre::TerrainMaterialGeneratorA::SM2Profile* matProfile;
	matProfile = static_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(
		mTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile()
		);

	//if (enableShadows)
	//{
		//  Make sure PSSM is already setup
		matProfile->setReceiveDynamicShadowsEnabled(true);
		matProfile->setReceiveDynamicShadowsPSSM(mPssmSetup);  // PSSM shadowing
		matProfile->setReceiveDynamicShadowsDepth(true);            // with depth
		matProfile->setReceiveDynamicShadowsLowLod(false);

		matProfile->setLightmapEnabled(false);

	//}
	//else
	//{
	//	matProfile->setReceiveDynamicShadowsPSSM(NULL);
	//}

	// Configure default import settings for if we use imported image
	Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = TERRAIN_SIZE;
	defaultimp.worldSize = TERRAIN_WORLD_SIZE;
	defaultimp.inputScale = 600;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	defaultimp.inputFloat = nullptr;
	defaultimp.inputImage = nullptr;

	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 120;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 300;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");


}

void CTerrain::DefineTerrain(long x, long y)
{
	const uint16 terrainSize = mTerrainGroup->getTerrainSize();
	float* heightMap = OGRE_ALLOC_T(float, terrainSize*terrainSize, MEMCATEGORY_GEOMETRY);

	Vector2 worldOffset(Real(x*(terrainSize - 1)), Real(y*(terrainSize - 1)));
	worldOffset += mOriginPoint;

	Vector2 revisedValuePoint;

	for (uint16 i = 0; i < terrainSize; i++)
	{
		for (uint16 j = 0; j < terrainSize; j++)
		{
			revisedValuePoint = (worldOffset + Vector2(j, i)) / mCycle;

			float e = 0.0f;
			float heightScale = mHeightScale;
			float frequency = mFrequency;
			for (uint16 o = 0; o < mOctaves; o++)
			{
				e += SimplexNoise::noise(
					frequency * revisedValuePoint.x,
					frequency * revisedValuePoint.y)
					* heightScale;

				heightScale = heightScale * 0.5f;
				frequency   = frequency   * 2.0f;
			}

			e = std::pow(e, mPowerFactor);
			heightMap[i*terrainSize + j] = e;
		}
	}
	mTerrainGroup->defineTerrain(x, y, heightMap);
}

void CTerrain::UpdateTerrainHeight(long x, long y)
{
	int simplexSeed = rand() % 50000;
	SimplexNoise::createPermutations(simplexSeed);

	const uint16 terrainSize = mTerrainGroup->getTerrainSize();
	float* heightMap = OGRE_ALLOC_T(float, terrainSize*terrainSize, MEMCATEGORY_GEOMETRY);

	Vector2 worldOffset(Real(x*(terrainSize - 1)), Real(y*(terrainSize - 1)));
	worldOffset += mOriginPoint;

	Vector2 revisedValuePoint = Vector2::ZERO;

	for (uint16 i = 0; i < terrainSize; i++)
	{
		for (uint16 j = 0; j < terrainSize; j++)
		{
			revisedValuePoint = (worldOffset + Vector2(j, i)) / mCycle;

			float e			  = 0.0f;
			float heightScale = mHeightScale;
			float frequency   = mFrequency;

			for (uint16 o = 0; o < mOctaves; o++)
			{
				e += SimplexNoise::noise(
					frequency * revisedValuePoint.x,
					frequency * revisedValuePoint.y)
					* heightScale;

				heightScale = heightScale * 0.5f;
				frequency   = frequency   * 2.0f;
			}

			e = std::pow(e, mPowerFactor);
			mTerrain->setHeightAtPoint(i, j, e * 500);
		}
	}
	mTerrain->update();
	mTerrain->getMaterial()->_dirtyState();
	mTerrain->getMaterial()->reload();
	mTerrainGroup->update();

	InitBlendMaps(mTerrain);
}

void CTerrain::GetTerrainImage(bool flipX, bool flipY, Image& img)
{
	//! [heightmap]
	img.load("terrain.png", mTerrainGroup->getResourceGroup());
	if (flipX)
		img.flipAroundY();
	if (flipY)
		img.flipAroundX();
	//! [heightmap]
}

void CTerrain::InitBlendMaps(Terrain* terrain)
{
	//! [blendmap]
	TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	float minHeight0 = 5;
	float fadeDist0 = 15;
	float minHeight1 = 200;
	float fadeDist1 = 15;
	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1 = blendMap1->getBlendPointer();
	for (uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			float height = terrain->getHeightAtTerrainPosition(tx, ty);
			
			*pBlend0++ = Math::saturate((height - minHeight0) / fadeDist0);
			*pBlend1++ = Math::saturate((height - minHeight1) / fadeDist1);
		}
	}

	std::cout << "Blendmap size: " << terrain->getLayerBlendMapSize() << std::endl;

	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}



void CTerrain::CreateTerrain()
{
	if (mSeed.empty())
	{
		srand(time(NULL));
	}
	else
	{
		std::hash<std::string> hashFunc;
		int32_t seedHash = hashFunc(mSeed);
		srand(seedHash);
	}

	int simplexSeed = rand() % 50000;
	SimplexNoise::createPermutations(simplexSeed);

	for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
	{
		for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
		{

			DefineTerrain(x, y);
		}

	}
	// sync load since we want everything in place when we start
	//mTerrainGroup->updateGeometry();
	mTerrainGroup->loadAllTerrains(true);
	mTerrain = mTerrainGroup->getTerrain(0, 0);
	//mTerrain->dirty();

	TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
	while (ti.hasMoreElements())
	{
		Terrain* t = ti.getNext()->instance;
		InitBlendMaps(t);
	}

}

void CTerrain::FlattenTerrainUnderObject(SceneNode * sn)
{
	//SceneNode* character = mSceneManager->getSceneNode("Character");
	AxisAlignedBox aabb = sn->_getWorldAABB();

	std::array<HeightmapCorrectionData, 4> flattenDataArray;

	flattenDataArray[0].corner = aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_BOTTOM);
	flattenDataArray[1].corner = aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_RIGHT_BOTTOM);
	flattenDataArray[2].corner = aabb.getCorner(AxisAlignedBox::CornerEnum::FAR_LEFT_BOTTOM);
	flattenDataArray[3].corner = aabb.getCorner(AxisAlignedBox::CornerEnum::FAR_RIGHT_BOTTOM);


	for (int i = 0; i < flattenDataArray.size(); i++)
	{
		flattenDataArray[i].terrainPosition = mTerrain->convertPosition(Terrain::Space::WORLD_SPACE, flattenDataArray[i].corner, Terrain::Space::POINT_SPACE);
	}

	float d1 = flattenDataArray[1].terrainPosition.x - flattenDataArray[0].terrainPosition.x;
	float d2 = flattenDataArray[2].terrainPosition.y - flattenDataArray[0].terrainPosition.y;

	Vector3 halfSize = aabb.getHalfSize();

	float offset1 = 0;//halfSize.x;
	float offset2 = 0;//halfSize.y;
	d1 += offset1;
	d2 += offset2;

	for (int a = 0; a < (d1 + offset1); a++)
	{
		for (int b = 0; b < (d2 + offset2); b++)
		{
			Vector3 pos = Vector3(
				flattenDataArray[0].terrainPosition.x + a, //- (offset1) + (halfSize.x * 0.5f), 
				flattenDataArray[0].terrainPosition.y + b, //- (offset2) + (halfSize.y * 0.5f), 
				flattenDataArray[0].corner.y/* + 5.0f*/);

			mTerrain->setHeightAtPoint(pos.x, pos.y, pos.z);

			//std::cout << "Flattened terrain at point: " << pos << " with index: " << (a + b) << std::endl;
		}

	}
	mTerrain->update();
}
