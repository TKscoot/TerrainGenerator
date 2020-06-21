#include "PoissonMeshInstance.h"

//static const char *c_materialsTechniques[] =
//{
//	"Examples/Instancing/RTSS/Robot",
//	"Examples/Instancing/VTF/Grass",
//	"Examples/Instancing/HWBasic/Robot",
//	"Examples/Instancing/VTF/HW/Robot",
//	"Examples/Instancing/VTF/HW/LUT/Robot",
//	"Examples/Instancing/RTSS/Robot"
//};

//static const char *c_meshNames[] =
//{
//	"grass",
//	"GreenTree.mesh",
//	"robot.mesh",
//	"spine.mesh"
//};

//------------------------------------------------------------------------------
void CPoissonMeshInstance::SetupInstanceManager()
{
	if (mCurrentManager)
		mSceneMgr->destroyInstanceManager(mCurrentManager);

	if (!mTechniqueIsSupported)
	{
		//Hide GUI features available only to instancing
		std::cout << "Instancing technique not supported by GPU! Falling back to standard Entity Creation." << std::endl;
		
		//TODO Non-instancing
		//createEntities();
		mCurrentManager = 0;
		return;
	}
	else
	{
		mCurrentManager = mSceneMgr->createInstanceManager(
			"InstanceMgr_" + mMeshName, mMeshName,
			ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, mInstancingTechnique,
			NUM_INST_ROW * NUM_INST_COLUMN, 0);

		CreateInstancedEntities();
	}
}

//------------------------------------------------------------------------------
void CPoissonMeshInstance::CreateInstancedEntities()
{
	std::mt19937 orng; // separate oriention rng for consistency with other techniques

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
		scale = Math::RangeRandom(0.5f, 1.0f);

		if (height >= mMinHeight && height <= mMaxHeight)
		{
			InstancedEntity* ent = mCurrentManager->createInstancedEntity(mMaterialName);
			mEntities.push_back(ent);

			if ((mInstancingTechnique < NUM_TECHNIQUES))
			{
				ent->setOrientation(Quaternion(Radian(float(orng()) / orng.max() * 10 * Math::PI), Vector3::UNIT_Y));
				ent->setPosition(position);
				ent->setScale(Vector3(scale));
			}
		}

	}

	DefragmentBatches();
}

//------------------------------------------------------------------------------
void CPoissonMeshInstance::ClearScene()
{
	std::vector<MovableObject*>::const_iterator itor = mEntities.begin();
	std::vector<MovableObject*>::const_iterator end = mEntities.end();

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

	mEntities.clear();
}
//-----------------------------------------------------------------------------------
void CPoissonMeshInstance::DestroyManagers()
{
	mSceneMgr->destroyInstanceManager(mCurrentManager);;
}

//------------------------------------------------------------------------------
void CPoissonMeshInstance::CleanupContent()
{
	ClearScene();
	DestroyManagers();
}

//------------------------------------------------------------------------------
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

