#include "PlantPlacer.h"
#include "Common/HeightFunction.h"

void CPlantPlacer::Initialize()
{
	CEventHandler::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CPlantPlacer::Update, this, std::placeholders::_1));

	HeightFunction::initialize(mTerrain);
	CreateGrassMesh();

	mInstancedGrass = new CPoissonMeshInstance(
		mSceneManager,
		"grass",
		"Examples/Instancing/VTF/Grass",
		mTerrain,
		true,
		InstanceManager::InstancingTechnique::TextureVTF,
		0.2f,
		1.5f);

	int terrainSize = mTerrain->getSize();
	std::vector<float> covMap;
	for (int x = 0; x < terrainSize; x++)
	{
		for (int y = 0; y < terrainSize; y++)
		{
			covMap.push_back(SimplexNoise::noise(x, y));
		}
	}

	mInstancedRocks = new CPoissonMeshInstance(
		mSceneManager,
		"Rock3.mesh",
		"Examples/Instancing/VTF/HW/Rock3",
		mTerrain,
		true,
		InstanceManager::HWInstancingVTF,
		0.5f,
		8.5f,
		120.0f,
		400.0f,
		8000.0f,
		covMap,
		0.8f);

	std::vector<String> treeArray;
	treeArray.push_back("fir05_30.mesh");
	treeArray.push_back("fir06_30.mesh");
	treeArray.push_back("fir14_25.mesh");

	mTree = new CPoissonMeshInstance(
		mSceneManager,
		treeArray,
		"placeholder",
		mTerrain,
		false,
		InstanceManager::HWInstancingVTF,
		0.7f,
		1.8f,
		80.0f,
		60.0f,
		700.0f,
		covMap,
		0.8f);

	std::vector<String> foliageArray;
	foliageArray.push_back("plant1.mesh");
	foliageArray.push_back("plant2.mesh");
	foliageArray.push_back("farn1.mesh");
	foliageArray.push_back("farn2.mesh");
	foliageArray.push_back("shroom1_1.mesh");
	foliageArray.push_back("shroom1_2.mesh");
	foliageArray.push_back("shroom1_3.mesh");
	foliageArray.push_back("shroom2_1.mesh");
	foliageArray.push_back("shroom2_2.mesh");
	foliageArray.push_back("shroom2_3.mesh");

	mFoliage = new CPoissonMeshInstance(
		mSceneManager,
		foliageArray,
		"placeholder",
		mTerrain,
		false,
		InstanceManager::HWInstancingVTF,
		0.7f,
		5.4f,
		80.0f,
		50.0f,
		850.0f);


	// BIOMES

	//mBiomeVegDescs = mBiomeHandler->GetBiomeVegetationDescriptions();
	mBiomeVegDescs = mBiomeHandler->mBiomeVegetationDescriptions;

	//mBiomeVegDescs[0].meshes.push_back("fir05_30.mesh");
	//mBiomeVegDescs[0].meshes.push_back("fir06_30.mesh");
	//mBiomeVegDescs[0].meshes.push_back("fir14_25.mesh");
	//mBiomeVegDescs[1].meshes.push_back("plant1.mesh");
	//mBiomeVegDescs[2].meshes.push_back("farn1.mesh");
	//mBiomeVegDescs[3].meshes.push_back("shroom1_1.mesh");

	for (int i = 0; i < mBiomeVegDescs.size(); i++)
	{
		mBiomeVegDescs[i].meshInstance = new CPoissonMeshInstance(
			mSceneManager,
			mBiomeVegDescs[i].meshes,
			"placeholder",
			mTerrain,
			false,
			InstanceManager::HWInstancingVTF,
			mBiomeVegDescs[i].minSize,
			mBiomeVegDescs[i].maxSize,
			mBiomeVegDescs[i].poissonRadius,
			-1000.0f,
			2500.0f,
			mBiomeVegDescs[i].coverageMap
		);
	}



}

void CPlantPlacer::Finalize()
{
	mInstancedGrass->ClearEntities();
	delete mInstancedGrass;
	mInstancedGrass = nullptr;

	mInstancedRocks->ClearEntities();
	delete mInstancedRocks;
	mInstancedRocks = nullptr;

	mTree->ClearEntities();
	delete mTree;
	mTree = nullptr;

	mFoliage->ClearEntities();
	delete mFoliage;
	mFoliage = nullptr;
}

void CPlantPlacer::PlaceVegetation()
{
	for (int i = 0; i < mBiomeVegDescs.size(); i++)
	{
		mBiomeVegDescs[i].meshInstance->UpdateCoverageMap(mBiomeHandler->mBiomeVegetationDescriptions[i].coverageMap);
	}

	ClearVegetation();

	//mInstancedGrass->PlaceEntities();
	//mInstancedRocks->PlaceEntities();
	//mTree->PlaceEntities();
	//mFoliage->PlaceEntities();

	mBiomeVegDescs[3].meshInstance->PlaceEntities(); // hill shrubs
	mBiomeVegDescs[4].meshInstance->PlaceEntities(); // shrubland
	mBiomeVegDescs[5].meshInstance->PlaceEntities(); // forest
	//mBiomeVegDescs[6].meshInstance->PlaceEntities(); // Autumn (Temperate_deciduous_forest)
	mBiomeVegDescs[7].meshInstance->PlaceEntities(); // jungle
	mBiomeVegDescs[8].meshInstance->PlaceEntities(); // desert cactus
	mBiomeVegDescs[9].meshInstance->PlaceEntities();
	mBiomeVegDescs[10].meshInstance->PlaceEntities(); // cactus
	mBiomeVegDescs[11].meshInstance->PlaceEntities(); // grassland

	//for (int i = 0; i < mBiomeVegDescs.size(); i++)
	//{
	//	mBiomeVegDescs[i].meshInstance->PlaceEntities();
	//}
}

void CPlantPlacer::ClearVegetation()
{
	mInstancedGrass->ClearEntities();
	mInstancedRocks->ClearEntities();
	mTree->ClearEntities();
	mFoliage->ClearEntities();

	for (int i = 0; i < mBiomeVegDescs.size(); i++)
	{
		mBiomeVegDescs[i].meshInstance->ClearEntities();
	}
}

void CPlantPlacer::CreateGrassMesh()
{
	MeshPtr mesh = MeshManager::getSingleton().createManual("grass", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// create a submesh with the grass material
	SubMesh* sm = mesh->createSubMesh();
	sm->setMaterialName("Examples/Instancing/VTF/Grass");
	sm->useSharedVertices = false;
	sm->vertexData = OGRE_NEW VertexData();
	sm->vertexData->vertexStart = 0;
	sm->vertexData->vertexCount = 12;
	sm->indexData->indexCount = 18;

#if defined(INCLUDE_RTSHADER_SYSTEM)
	MaterialPtr grassMat = MaterialManager::getSingleton().getByName("Examples/Instancing/VTF/Grass");
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


	auto matName = sm->getMaterialName(); // MaterialName is an Ogre::String
	MaterialPtr matPtr = MaterialManager::getSingleton().getByName(matName); // pMaterial is an Ogre::MaterialPtr
	matPtr->getTechnique(0)->getPass(0)->setTransparentSortingEnabled(true);
	matPtr->getTechnique(0)->getPass(0)->setTransparentSortingForced(true);
}