#include "Terrain.h"


CTerrain::~CTerrain()
{
	delete mBiomeHandler;
	mBiomeHandler = nullptr;

	delete mErosion;
	mErosion = nullptr;

	delete mTerrainGroup;
	mTerrainGroup = nullptr;

	delete mTerrainGlobals;
	mTerrainGlobals = nullptr;
}

void CTerrain::Initialize(PSSMShadowCameraSetup* pssmSetup)
{
	CEventHandler::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CTerrain::Update, this, std::placeholders::_1));

	mTerrainGlobals = new Ogre::TerrainGlobalOptions();
	mTerrainGroup   = new Ogre::TerrainGroup(mSceneManager, Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
	mTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
	mTerrainGroup->setOrigin(mTerrainPos);

	Ogre::Light* l = mSceneManager->createLight("terrainLight");
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDiffuseColour(ColourValue::White);
	l->setSpecularColour(ColourValue(0.4, 0.4, 0.4));
	Ogre::SceneNode* ln = mSceneManager->getRootSceneNode()->createChildSceneNode();
	ln->setDirection(Vector3(0.55, -0.9, 0.45).normalisedCopy());
	ln->setDirection(Vector3::NEGATIVE_UNIT_Z);
	ln->attachObject(l);


	ConfigureTerrainDefaults();

	CreateTerrain();
	
	mBiomeHandler = new CBiomeHandler(mTerrain);
	mBiomeHandler->UpdateCoverageMap();
	mPlantPlacer = new CPlantPlacer(mSceneManager, mTerrain, mBiomeHandler);
	mPlantPlacer->Initialize();

	mPlantPlacer->PlaceVegetation();

	std::function<void(int)> erodeFunction = std::bind(&CTerrain::Erode, this, std::placeholders::_1);
	mErosion = new CHydraulicErosion(mTerrainGroup->getTerrainSize(), erodeFunction);

	mTerrainMaterial->generate(mTerrain);



}

bool CTerrain::Update(const FrameEvent& evt)
{
	// GUI
	std::ostringstream ss;
	const char* str0 = mSeed.c_str();
	char* str = const_cast<char*>(str0);
	char str1[255];
	memcpy(str1, str0, IM_ARRAYSIZE(str1));

	if (ImGui::CollapsingHeader("Terrain", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginChild("Terrain", ImVec2(0, 185), true);
		if (ImGui::InputText("Seed", str1, IM_ARRAYSIZE(str1)))
		{
			ss << str1;
			mSeed = ss.str();
		}
		ImGui::SliderInt  ("Octaves",      &mOctaves,     1,     15);
		ImGui::SliderFloat("Frequency",	   &mFrequency,   0.05f, 5.0f);
		ImGui::SliderFloat("Height Scale", &mHeightScale, 0.05f, 2.5f);
		ImGui::SliderFloat("Power factor", &mPowerFactor, 0.1f,  3.0f);
		ImGui::Spacing();
		ImGui::Checkbox("Place vegetation on generation", &mPlaceVegetation);
		ImGui::Spacing();
		if (ImGui::Button("Generate!"))
		{
			UpdateTerrainHeight(0, 0);
			mBiomeHandler->UpdateCoverageMap();

			Sleep(3000);

			mTerrainMaterial->generate(mTerrain);
			mTerrain->getMaterial()->compile();
			mTerrain->getMaterial()->unload();
			mTerrain->getMaterial()->load();
			mTerrain->getMaterial()->reload();

			if (mPlaceVegetation)
			{
				mPlantPlacer->PlaceVegetation();
			}
			else
			{
				mPlantPlacer->ClearVegetation();
			}
		}
		ImGui::EndChild();
	}
	ImGui::Spacing();
	return true;
}

void CTerrain::Erode(int iterations)
{
	// reseed rand
	srand(time(NULL));

	const uint16 terrainSize = mTerrainGroup->getTerrainSize();

	// copy map to vector for easier use
	float* map = mTerrain->getHeightData();
	std::vector<float>vMap(map, map + (terrainSize * terrainSize));

	// normalise height points
	float minValue = std::numeric_limits<float>::max();
	float maxValue = std::numeric_limits<float>::min();

	for (int i = 0; i < vMap.size(); i++)
	{
		minValue = std::min(vMap[i], minValue);
		maxValue = std::max(vMap[i], maxValue);
	}

	for (int i = 0; i < vMap.size(); i++)
	{
		vMap[i] = (vMap[i] - minValue) / (maxValue - minValue);
	}

	// calculate erosion
	for (int i = 0; i < iterations; i++)
	{
		mErosion->Calculate(vMap);
	}

	// "denormalise" height points
	for (uint16 i = 0; i < terrainSize; i++)
	{
		for (uint16 j = 0; j < terrainSize; j++)
		{
			int it = i * terrainSize + j;

			map[it] = (vMap[it] + minValue) * (maxValue + minValue);
		}
	}

	mTerrain->dirty();
	mTerrain->update();
}

void CTerrain::ConfigureTerrainDefaults(/*Light * l*/)
{
	// Configure global
	mTerrainGlobals->setMaxPixelError(8);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(30000);

	Light* l = mSceneManager->getLight("terrainLight");

	// Custom Terrain Material for splatting Textures
	// Set Ogre Material  with the name "TerrainMaterial" in constructor
	mTerrainMaterial = OGRE_NEW TerrainMaterial("CustomTerrain");
	mTerrainMaterial->setMaterialByName("CustomTerrain");
	TerrainMaterialGeneratorPtr terrainMaterialGenerator(mTerrainMaterial);
	//terrainMaterialGenerator.reset<TerrainMaterial>(mTerrainMaterial);
	
	mTerrainGlobals->setDefaultMaterialGenerator(terrainMaterialGenerator);


	// Important to set these so that the terrain knows what to use for derived data
	mTerrainGlobals->setUseVertexCompressionWhenAvailable(false);
	mTerrainGlobals->setCastsDynamicShadows(true);
	mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(mSceneManager->getAmbientLight());
	mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());
	mTerrainGlobals->setUseRayBoxDistanceCalculation(true);

	// Configure default 
	Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize  = TERRAIN_SIZE;
	defaultimp.worldSize    = TERRAIN_WORLD_SIZE;
	defaultimp.inputScale   = 600;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	defaultimp.inputFloat   = nullptr;
	defaultimp.inputImage   = nullptr;

	// textures
	defaultimp.layerList.resize(4);
	defaultimp.layerList[0].worldSize = 120;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 120;
	defaultimp.layerList[1].textureNames.push_back("Sand_baseColor.png");
	defaultimp.layerList[1].textureNames.push_back("Sand_normal.png");
	defaultimp.layerList[2].worldSize = 300;
	defaultimp.layerList[2].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[3].worldSize = 800;
	defaultimp.layerList[3].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[3].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
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
	UpdateSeed();

	const uint16 terrainSize = mTerrainGroup->getTerrainSize();

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

	// update terrain and materials
	mTerrain->update();
	mTerrain->getMaterial()->_dirtyState();
	mTerrain->getMaterial()->reload();
	mTerrainGroup->update();

	InitBlendMaps(mTerrain);
}

void CTerrain::InitBlendMaps(Terrain* terrain)
{	
	// Configure Blendmaps
	TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	TerrainLayerBlendMap* blendMap2 = terrain->getLayerBlendMap(3);
	float  minHeight0 = 5;
	float  fadeDist0  = 15;
	float  minHeight1 = 75;
	float  fadeDist1  = 15;
	float  minHeight2 = 700;
	float  fadeDist2 = 50;
	float* pBlend0    = blendMap0->getBlendPointer();
	float* pBlend1    = blendMap1->getBlendPointer();
	float* pBlend2    = blendMap2->getBlendPointer();

	for (uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			float height = terrain->getHeightAtTerrainPosition(tx, ty);

			*pBlend0++ = Math::saturate((height - minHeight0) / fadeDist0);
			*pBlend1++ = Math::saturate((height - minHeight1) / fadeDist1);
			*pBlend2++ = Math::saturate((height - minHeight2) / fadeDist2);
		}
	}

	blendMap0->dirty();
	blendMap1->dirty();
	blendMap2->dirty();
	blendMap0->update();
	blendMap1->update();
	blendMap2->update();
}

void CTerrain::CreateTerrain()
{
	UpdateSeed();

	for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
	{
		for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
		{

			DefineTerrain(x, y);
		}

	}

	// sync load since we want everything in place when we start
	mTerrainGroup->loadAllTerrains(true);
	mTerrain = mTerrainGroup->getTerrain(0, 0);

	TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
	while (ti.hasMoreElements())
	{
		Terrain* t = ti.getNext()->instance;
		InitBlendMaps(t);
	}
}

void CTerrain::UpdateSeed()
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
}
