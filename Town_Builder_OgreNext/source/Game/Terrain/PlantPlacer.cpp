#include "PlantPlacer.h"
#include "Common/HeightFunction.h"

void CPlantPlacer::Initialize()
{
	CEventHandler::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CPlantPlacer::Update, this, std::placeholders::_1));

	HeightFunction::initialize(mTerrain);
	CreateGrassMesh();

	// Biome Vegetation
	mBiomeVegDescs = mBiomeHandler->mBiomeVegetationDescriptions;

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

	for (int i = 0; i < mBiomeVegDescs.size(); i++)
	{
		if (mBiomeVegDescs[i].meshes.empty())
		{
			mBiomeNames.push_back("No Mesh");

		}
		else
		{
			mBiomeNames.push_back(mBiomeVegDescs[i].meshes[0].c_str());
		}
	}

}

void CPlantPlacer::Finalize()
{
	// Delete all mesh instances
	for (int i = 0; i < mBiomeVegDescs.size(); i++)
	{
		delete mBiomeVegDescs[i].meshInstance;
		mBiomeVegDescs[i].meshInstance = nullptr;
	}
}

bool CPlantPlacer::Update(const FrameEvent &evt)
{
	static int selectedItem = 0;
	if (ImGui::CollapsingHeader("Vegetation", ImGuiTreeNodeFlags_DefaultOpen))
	{

		ImGui::Combo("Biomes", &selectedItem, mBiomeNames.data(), mBiomeNames.size());

		ImGui::BeginChild(mBiomeNames[selectedItem], ImVec2(0, 136), true);
		if (mBiomeNames[selectedItem] != "No Mesh")
		{
			ImGui::SliderFloat("Radius",    &mBiomeVegDescs[selectedItem].poissonRadius, 20.0f, 500.0f);
			ImGui::SliderFloat("Min scale", &mBiomeVegDescs[selectedItem].minSize,        0.0f, 100.0f);
			ImGui::SliderFloat("Max scale", &mBiomeVegDescs[selectedItem].maxSize,        0.0f, 100.0f);
			ImGui::Spacing();

			if (ImGui::Button("Place meshes"))
			{
				mBiomeVegDescs[selectedItem].meshInstance->UpdateParameters(
					mBiomeVegDescs[selectedItem].minSize,
					mBiomeVegDescs[selectedItem].maxSize,
					mBiomeVegDescs[selectedItem].poissonRadius);

				for (int i = 0; i < mBiomeVegDescs.size(); i++)
				{
					mBiomeVegDescs[i].meshInstance->UpdateCoverageMap(mBiomeHandler->mBiomeVegetationDescriptions[i].coverageMap);
				}

				mBiomeVegDescs[selectedItem].meshInstance->ClearEntities();
				mBiomeVegDescs[selectedItem].meshInstance->PlaceEntities();
			}

			if (ImGui::Button("Clear meshes"))
			{
				mBiomeVegDescs[selectedItem].meshInstance->ClearEntities();
			}
		}
		else
		{
			ImGui::Text("No meshes to place for this biome!");
		}

		ImGui::EndChild();
	}
	return true;
}

void CPlantPlacer::PlaceVegetation()
{
	for (int i = 0; i < mBiomeVegDescs.size(); i++)
	{
		mBiomeVegDescs[i].meshInstance->UpdateCoverageMap(mBiomeHandler->mBiomeVegetationDescriptions[i].coverageMap);
	}

	ClearVegetation();

	mBiomeVegDescs[3].meshInstance->PlaceEntities(); // hill shrubs
	mBiomeVegDescs[4].meshInstance->PlaceEntities(); // shrubland
	mBiomeVegDescs[5].meshInstance->PlaceEntities(); // forest
	mBiomeVegDescs[7].meshInstance->PlaceEntities(); // jungle
	mBiomeVegDescs[8].meshInstance->PlaceEntities(); // desert cactus
	mBiomeVegDescs[9].meshInstance->PlaceEntities();
	mBiomeVegDescs[10].meshInstance->PlaceEntities(); // cactus
	mBiomeVegDescs[11].meshInstance->PlaceEntities(); // grassland
}

void CPlantPlacer::ClearVegetation()
{
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