#include "PlantPlacer.h"
#include "Common/HeightFunction.h"

void CPlantPlacer::Initialize()
{
	CEventHandler::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CPlantPlacer::Update, this, std::placeholders::_1));

	CreateGrassMesh();
	mInstancingManager = new CPoissonMeshInstance(mSceneManager, "grass", "Examples/Instancing/VTF/Grass", mTerrain, InstanceManager::InstancingTechnique::TextureVTF);

	// TODO ROCK INSTANCING
	//mInstancingManager = new CPoissonMeshInstance(mSceneManager, "rock", "Examples/Instancing/VTF/Grass", mTerrain);


	//PlaceTrees();
}

void CPlantPlacer::PlaceTrees()
{
	const std::array<float, 2> kXMin = std::array<float, 2>{ { -6000.0f, -6000.0f }};
	const std::array<float, 2> kXMax = std::array<float, 2>{ { 6000.0f, 6000.0f }};

	const auto samples = thinks::PoissonDiskSampling(mPoissonRadius, kXMin, kXMax);

	float   height;
	Vector3 position;
	Degree  yaw;
	float   scale;

	for (auto& sample : samples)
	{
		height = mTerrain->getTerrainGroup()->getHeightAtWorldPosition(Vector3(sample[0], 0, sample[1]));
		position = Vector3(sample[0], height, sample[1]);
		yaw = Degree(Math::RangeRandom(0, 360));
		scale = Math::RangeRandom(10.0f, 15.0f);

		if (height >= 50 && height <= 450)
		{
			Entity* ent = mSceneManager->createEntity("GreenTree.mesh");
			ent->setMaterialLodBias(1.0);
			ent->setMeshLodBias(1.0);
			SceneNode* entNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
			entNode->attachObject(ent);
			entNode->setPosition(position);
			entNode->setScale(Vector3(scale));
			entNode->yaw(yaw);
			
			mPlacedTrees.push_back(entNode);
		}

	}
	
	std::cout << "Placed " << mPlacedTrees.size() << " Trees without instancing!" << std::endl;
}

void CPlantPlacer::ClearTrees()
{
	for (int i = 0; i < mPlacedTrees.size(); i++)
	{
		mPlacedTrees[i]->removeAndDestroyAllChildren();
		mSceneManager->destroySceneNode(mPlacedTrees[i]);
	}

	mPlacedTrees.clear();
}

bool CPlantPlacer::Update(const FrameEvent &evt)
{
	if (ImGui::CollapsingHeader("TreePlacement", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginChild("TreePlacement", ImVec2(0, 135), true);
		ImGui::SliderFloat("Radius",	   &mPoissonRadius, 20.0f,  500.0f);
		ImGui::SliderFloat("Min height",   &mMinHeight,      0.0f, 1000.0f);
		ImGui::SliderFloat("Max height",   &mMaxHeight,      0.0f, 1000.0f);
		ImGui::Spacing();
		if (ImGui::Button("Place trees!"))
		{
			ClearTrees();
			PlaceTrees();
		}
		ImGui::EndChild();
	}
	
	ImGui::End();

	return true;
}

void CPlantPlacer::Finalize()
{
	//delete mTreePG->getPageLoader();
	//delete mTreePG;
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


	//auto matName = sm->getMaterialName(); // MaterialName is an Ogre::String
	//MaterialPtr matPtr = MaterialManager::getSingleton().getByName(matName); // pMaterial is an Ogre::MaterialPtr
	//matPtr->getTechnique(0)->getPass(0)->setTransparentSortingEnabled(true);
	//matPtr->getTechnique(0)->getPass(0)->setTransparentSortingForced(true);
	//matPtr->getTechnique(0)->getPass(0)->setDiffuse(r, g, b, a);

}