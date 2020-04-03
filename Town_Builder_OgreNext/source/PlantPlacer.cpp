#include "PlantPlacer.h"
#include "HeightFunction.h"


void CPlantPlacer::Initialize()
{
	//HeightFunction::initialize(mTerrain->getTerrainGroup());

	//PlaceTrees();
	//PlaceGrass();
}

void CPlantPlacer::PlaceTrees()
{
	mTreesPG->setCamera(mSceneManager->getCamera("Camera"));

	mTreesPG->setPageSize(150);
	mTreesPG->setInfinite();

	mTreesPG->addDetailLevel<BatchPage>(4000, 30);
	mTreesPG->addDetailLevel<ImpostorPage>(8000, 50);

	AxisAlignedBox terrainAABB = mTerrain->getTerrainGroup()->getTerrain(0, 0)->getWorldAABB();
	TBounds bounds = mTreesPG->convertAABToTBounds(terrainAABB);

	mTreeLoader = new TreeLoader3D(mTreesPG, bounds);
	mTreesPG->setPageLoader(mTreeLoader);
	mTreeLoader->setMaximumScale(10);

	mEntity = mSceneManager->createEntity("Tree", "tree.mesh");

	HeightFunction::initialize(mTerrain->getTerrainGroup());

	Vector3 position;
	Radian yaw;
	Real scale;

	for (int x = bounds.left; x < bounds.right; x++)
	{
		for (int z = bounds.left; z < bounds.right; z++)
		{

			position.x = x;
			position.z = z;
			position.y = mTerrain->getTerrainGroup()->getHeightAtWorldPosition(position);


			if (position.y > 5 && position.y < 200)
			{
				int rand = Math::RangeRandom(0, 10);
				if (rand == 10)
				{

					yaw = Degree(Math::RangeRandom(0, 360));
					scale = Math::RangeRandom(5, 10.0f);
					//scale = 10.0f;

					mTreeLoader->addTree(mEntity, position, yaw, scale);
				}

			}

		}

	}




	//CreateGrassMesh();
	//Entity* grass = mSceneManager->createEntity("Grass", "grass");


	//StaticGeometry* sg = mSceneManager->createStaticGeometry("Field");
	//for (int x = bounds.left; x < bounds.right; x += Math::RangeRandom(10, 50))
	//{
	//	for (int z = bounds.left; z < bounds.right; z += Math::RangeRandom(10, 50))
	//	{
	//		Vector3 pos(x + Math::RangeRandom(-7, 7), 0, z + Math::RangeRandom(-10, 10));
	//		pos.y = mTerrain->getTerrainGroup()->getHeightAtWorldPosition(pos);
	//
	//		if (pos.y > 5 && pos.y < 200)
	//		{
	//		
	//			Quaternion ori(Degree(Math::RangeRandom(0, 359)), Vector3::UNIT_Y);
	//			float randScale = Math::RangeRandom(2.85, 4.15);
	//			Vector3 scale(randScale, randScale, randScale);
	//
	//			sg->addEntity(mEntity, pos, ori, scale);
	//		}
	//	}
	//}


	//for (int x = bounds.left; x < bounds.right; x += Math::RangeRandom(10, 50))
	//{
	//	for (int z = bounds.left; z < bounds.right; z += Math::RangeRandom(10, 50))
	//	{
	//		Vector3 pos(x + Math::RangeRandom(-40, 40), 0, z + Math::RangeRandom(-40, 40));
	//		pos.y = mTerrain->getTerrainGroup()->getHeightAtWorldPosition(pos);
	//
	//		if (pos.y > 5 && pos.y < 200)
	//		{
	//
	//			Quaternion ori(Degree(Math::RangeRandom(0, 359)), Vector3::UNIT_Y);
	//			float randScale = Math::RangeRandom(0.85, 1.15);
	//			Vector3 scale(randScale, randScale, randScale);
	//
	//			sg->addEntity(grass, pos, ori, scale);
	//		}
	//	}
	//}

	//sg->build();


	
}

void CPlantPlacer::PlaceGrass()
{
	mGrassPG->setCamera(mSceneManager->getCamera("Camera"));
	mGrassPG->setPageSize(150);
	mGrassPG->setInfinite();
	mGrassPG->addDetailLevel<GrassPage>(1500);

	GrassLoader* grassLoader = new GrassLoader(mGrassPG);
	mGrassPG->setPageLoader(grassLoader);
	grassLoader->setHeightFunction(&HeightFunction::getTerrainHeight);

	GrassLayer* layer = grassLoader->addLayer("grass");
	layer->setHeightRange(20.0f, 180.0f);
	layer->setMinimumSize(5.0f, 5.0f);
	layer->setMaximumSize(5.5f, 5.5f);
	layer->setAnimationEnabled(false);
	layer->setSwayDistribution(10.0f);
	layer->setSwayLength(0.5f);
	layer->setSwaySpeed(0.5f);
	//layer->setDensity(0.4f);
	layer->setFadeTechnique(FADETECH_GROW);
	layer->setDensityMap("terrain.png", Forests::CHANNEL_COLOR);

	AxisAlignedBox terrainAABB = mTerrain->getTerrainGroup()->getTerrain(0, 0)->getWorldAABB();
	TBounds bounds = mGrassPG->convertAABToTBounds(terrainAABB);
	layer->setMapBounds(bounds);
	layer->setColorMap(mTerrain->getColorMap());
	layer->setMaxSlope(Degree(5.0f).valueRadians());
}

void CPlantPlacer::Update()
{
	//mTreesPG->update();
	//mGrassPG->update();
}

void CPlantPlacer::CreateGrassMesh()
{
	MeshPtr mesh = MeshManager::getSingleton().createManual("grass", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// create a submesh with the grass material
	SubMesh* sm = mesh->createSubMesh();
	sm->setMaterialName("Examples/GrassBlades");
	sm->useSharedVertices = false;
	sm->vertexData = OGRE_NEW VertexData();
	sm->vertexData->vertexStart = 0;
	sm->vertexData->vertexCount = 12;
	sm->indexData->indexCount = 18;

#if defined(INCLUDE_RTSHADER_SYSTEM)
	MaterialPtr grassMat = MaterialManager::getSingleton().getByName("Examples/GrassBlades");
	grassMat->getTechnique(0)->setSchemeName(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
#endif

	// specify a vertex format declaration for our mesh: 3 floats for position, 3 floats for normal, 2 floats for UV
	VertexDeclaration* decl = sm->vertexData->vertexDeclaration;
	decl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
	decl->addElement(0, sizeof(float) * 3, VET_FLOAT3, VES_NORMAL);
	decl->addElement(0, sizeof(float) * 6, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);

	// create a vertex buffer
	HardwareVertexBufferSharedPtr vb = HardwareBufferManager::getSingleton().createVertexBuffer
	(decl->getVertexSize(0), sm->vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	GrassVertex* verts = (GrassVertex*)vb->lock(HardwareBuffer::HBL_DISCARD);  // start filling in vertex data

	for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
	{
		// planes intersect along the Y axis with 60 degrees between them
		Real x = Math::Cos(Degree(i * 60)) * GRASS_WIDTH / 2;
		Real z = Math::Sin(Degree(i * 60)) * GRASS_WIDTH / 2;

		for (unsigned int j = 0; j < 4; j++)  // each plane has 4 vertices
		{
			GrassVertex& vert = verts[i * 4 + j];

			vert.x = j < 2 ? -x : x;
			vert.y = j % 2 ? 0 : GRASS_HEIGHT;
			vert.z = j < 2 ? -z : z;

			// all normals point straight up
			vert.nx = 0;
			vert.ny = 1;
			vert.nz = 0;

			vert.u = j < 2 ? 0 : 1;
			vert.v = j % 2;
		}
	}

	vb->unlock();  // commit vertex changes

	sm->vertexData->vertexBufferBinding->setBinding(0, vb);  // bind vertex buffer to our submesh

	// create an index buffer
	sm->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer
	(HardwareIndexBuffer::IT_16BIT, sm->indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	// start filling in index data
	Ogre::uint16* indices = (Ogre::uint16*)sm->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD);

	for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
	{
		unsigned int off = i * 4;  // each plane consists of 2 triangles

		*indices++ = 0 + off;
		*indices++ = 3 + off;
		*indices++ = 1 + off;

		*indices++ = 0 + off;
		*indices++ = 2 + off;
		*indices++ = 3 + off;
	}

	sm->indexData->indexBuffer->unlock();  // commit index changes
}
