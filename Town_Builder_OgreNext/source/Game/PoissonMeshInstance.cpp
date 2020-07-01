#include "PoissonMeshInstance.h"
#include "OgreLodStrategy.h"

void CPoissonMeshInstance::SetupInstanceManager()
{
	if (mCurrentManager)
		mSceneMgr->destroyInstanceManager(mCurrentManager);

	if (!mTechniqueIsSupported)
	{
		std::cout << "Instancing technique not supported by GPU! Falling back to standard Entity Creation." << std::endl;
		
		//TODO Non-instancing
		//createEntities();
		mCurrentManager = 0;
		mUseInstancing = false;
		return;
	}
	else
	{
		mUseInstancing = true;

		mCurrentManager = mSceneMgr->createInstanceManager(
			"InstanceMgr_" + mMeshName, mMeshName,
			ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, mInstancingTechnique,
			NUM_INST_ROW * NUM_INST_COLUMN, 0);
	}
}

void CPoissonMeshInstance::PlaceEntities()
{
	if (mUseInstancing)
	{
		CreateInstancedEntities();
	}
	else
	{
		CreateNonInstancedEntities();
	}

}

bool CPoissonMeshInstance::Update(const FrameEvent & evt)
{
	std::string tabLabel = "PoissonInstancing " + mMeshName;
	if (ImGui::CollapsingHeader(tabLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginChild(tabLabel.c_str(), ImVec2(0, 181), true);
		ImGui::SliderFloat("Radius", &mPoissonRadius, 20.0f, 500.0f);
		ImGui::SliderFloat("Min height", &mMinHeight, 0.0f, 1000.0f);
		ImGui::SliderFloat("Max height", &mMaxHeight, 0.0f, 1000.0f);
		ImGui::Spacing();
		ImGui::SliderFloat("Min scale", &mMinScale, 0.0f, 100.0f);
		ImGui::SliderFloat("Max scale", &mMaxScale, 0.0f, 100.0f);
		ImGui::Spacing();
		std::string placingLabel = "Place " + mMeshName;
		if (ImGui::Button(placingLabel.c_str()))
		{
			ClearEntities();
			PlaceEntities();
		}

		std::string clearingLabel = "Clear " + mMeshName;
		if (ImGui::Button(clearingLabel.c_str()))
		{
			ClearEntities();
		}

		ImGui::EndChild();
	}

	return true;
}

void CPoissonMeshInstance::CreateInstancedEntities()
{
	std::mt19937 orng; // separate oriention rng for consistency with other techniques

	float halfTerrainSize = (mTerrain->getWorldSize() / 2);
	const std::array<float, 2> kXMin = std::array<float, 2>{ { -halfTerrainSize, -halfTerrainSize }};
	const std::array<float, 2> kXMax = std::array<float, 2>{ { halfTerrainSize, halfTerrainSize }};

	const auto samples = thinks::PoissonDiskSampling(mPoissonRadius, kXMin, kXMax, 30, Math::RangeRandom(0, 50000));

	float   height	 = 0.0f;
	Vector3 position = Vector3::ZERO;
	float   scale	 = 1.0f;

	int i = 0;
	for (auto& sample : samples)
	{
		height = mTerrain->getHeightAtWorldPosition(Vector3(sample[0], 0, sample[1]));
		Vector3 terrainPos = Vector3::ZERO;
		mTerrain->convertPosition(Terrain::WORLD_SPACE, Vector3(sample[0], height, sample[1]), Terrain::POINT_SPACE, terrainPos);
		//std::cout << "worldPos: " << Vector3(sample[0], height, sample[1]) << " terrainPoint: " << terrainPos << std::endl;
		position = Vector3(sample[0], height, sample[1]);
		scale = Math::RangeRandom(mMinScale, mMaxScale);
		bool shouldPlace = false;
		int coverageMapSize = sqrt(mCoverageMap.size());
		if (mUseCoverageMap)
		{
			if ( mCoverageMap[terrainPos.x * coverageMapSize + terrainPos.y] >= mCoverageThreshold)
			{
				shouldPlace = true;
			}
			else
			{
				shouldPlace = false;
			}
		}
		else
		{
			shouldPlace = true;
		}

		if (shouldPlace && height >= mMinHeight && height <= mMaxHeight)
		{
			InstancedEntity* ent = mCurrentManager->createInstancedEntity(mMaterialName);
			mInstancedEntities.push_back(ent);

			if ((mInstancingTechnique < NUM_TECHNIQUES))
			{
				ent->setOrientation(Quaternion(Radian(float(orng()) / orng.max() * 10 * Math::PI), Vector3::UNIT_Y));
				ent->setPosition(position);
				ent->setScale(Vector3(scale));
			}
		}

		++i;
	}

	DefragmentBatches();
}

void CPoissonMeshInstance::CreateNonInstancedEntities()
{
	std::mt19937 orng; // separate oriention rng for consistency with other techniques

	float halfTerrainSize = (mTerrain->getWorldSize() / 2);
	const std::array<float, 2> kXMin = std::array<float, 2>{ { -halfTerrainSize, -halfTerrainSize }};
	const std::array<float, 2> kXMax = std::array<float, 2>{ { halfTerrainSize, halfTerrainSize }};

	const auto samples = thinks::PoissonDiskSampling(mPoissonRadius, kXMin, kXMax, 30, Math::RangeRandom(0, 50000));

	float   height = 0.0f;
	Vector3 position = Vector3::ZERO;
	float   scale = 1.0f;

	int i = 0;
	for (auto& sample : samples)
	{
		height = mTerrain->getHeightAtWorldPosition(Vector3(sample[0], 0, sample[1]));
		Vector3 terrainPos = Vector3::ZERO;
		mTerrain->convertPosition(Terrain::WORLD_SPACE, Vector3(sample[0], height, sample[1]), Terrain::POINT_SPACE, terrainPos);
		position = Vector3(sample[0], height, sample[1]);
		scale = Math::RangeRandom(mMinScale, mMaxScale);
		bool shouldPlace = false;
		int coverageMapSize = sqrt(mCoverageMap.size());
		if (mUseCoverageMap)
		{
			if (mCoverageMap[terrainPos.x * coverageMapSize + terrainPos.y] >= mCoverageThreshold)
			{
				shouldPlace = true;
			}
			else
			{
				shouldPlace = false;
			}
		}
		else
		{
			shouldPlace = true;
		}

		if (shouldPlace && height >= mMinHeight && height <= mMaxHeight)
		{
			Entity* ent = nullptr;
			if (mUseMeshVector)
			{
				ent = mSceneMgr->createEntity(mMeshNames[int(Ogre::Math::RangeRandom(0, mMeshNames.size() - 1))]);
			}
			else
			{
				ent = mSceneMgr->createEntity(mMeshName);
			}

			SceneNode* entNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			entNode->attachObject(ent);
			entNode->setPosition(position);
			entNode->setScale(Vector3(scale));
			entNode->setOrientation(Quaternion(Radian(float(orng()) / orng.max() * 10 * Math::PI), Vector3::UNIT_Y));
			ent->setMaterialLodBias(1.0, 0, 1);
			ent->setMeshLodBias(1.0, 0, 1);
			std::vector<SubMesh*> submeshes = ent->getMesh()->getSubMeshes();

			for (auto& m : submeshes)
			{
				Material::LodValueList lvl{ 0 };
			
			
				m->getMaterial()->setLodLevels(lvl);
				m->getMaterial()->compile();
			}

			mNonInstancedEntities.push_back(entNode);
		}

		++i;
	}
}

void CPoissonMeshInstance::ClearNonInstancedEntities()
{
	for(int i = 0; i < mNonInstancedEntities.size(); i++)
	{
		mNonInstancedEntities[i]->removeAndDestroyAllChildren();
		mSceneMgr->destroySceneNode(mNonInstancedEntities[i]);
	}

	mNonInstancedEntities.clear();
}

void CPoissonMeshInstance::ClearInstancedEntities()
{
	std::vector<MovableObject*>::const_iterator itor = mInstancedEntities.begin();
	std::vector<MovableObject*>::const_iterator end = mInstancedEntities.end();

	//Note: Destroying the instance manager automatically destroys all instanced entities
	//created by this manager (beware of not leaving reference to those pointers)
	while (itor != end)
	{
			mSceneMgr->destroyInstancedEntity(static_cast<InstancedEntity*>(*itor));

		++itor;
	}

	//Free some memory, but don't destroy the manager so when we switch this technique
	//back again it doesn't take too long
	if (mCurrentManager)
		mCurrentManager->cleanupEmptyBatches();

	mInstancedEntities.clear();
}

void CPoissonMeshInstance::DestroyManagers()
{
	mSceneMgr->destroyInstanceManager(mCurrentManager);;
}

void CPoissonMeshInstance::ClearEntities()
{
	ClearInstancedEntities();
	ClearNonInstancedEntities();
}

void CPoissonMeshInstance::DefragmentBatches()
{
	//Defragment batches is used after many InstancedEntities were removed (and you won't
	//be requesting more). However, then the optimize cull option is on, it can cause
	//quite a perf. boost on large batches (i.e. VTF) even if not a single instance was ever removed.
	if (mCurrentManager)
		mCurrentManager->defragmentBatches(true);
}

void CPoissonMeshInstance::TestCapabilities(const RenderSystemCapabilities * caps)
{
	if (!GpuProgramManager::getSingleton().isSyntaxSupported("glsl") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("glsl300es") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("fp40") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_0") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("ps_3_0") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_0") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_1") &&
		!GpuProgramManager::getSingleton().isSyntaxSupported("ps_5_0"))
	{
		OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your card does not support the shader model needed for this sample, "
			"so you cannot run this program. Sorry!", "NewInstancing::TestCapabilities");
	}
}

void CPoissonMeshInstance::CheckHardwareSupport()
{
	//Check Technique support
	for (int i = 0; i < NUM_TECHNIQUES; ++i)
	{
		InstanceManager::InstancingTechnique technique = mInstancingTechnique;

		uint16 flags = IM_USEALL;
		if (i == 4)
		{
			flags |= IM_VTFBONEMATRIXLOOKUP;
		}

		const size_t numInstances = mSceneMgr->getNumInstancesPerBatch(mMeshName,
			ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
			mMaterialName, technique, NUM_INST_ROW * NUM_INST_COLUMN, flags);

		mTechniqueIsSupported = numInstances > 0;
	}
}

