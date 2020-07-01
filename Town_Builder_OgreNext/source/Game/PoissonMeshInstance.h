#pragma once
#include <Ogre.h>
#include <random>
#include <iostream>

#include <imgui.h>

#include "Terrain/OgreTerrainMaterialGeneratorA.h"
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainGroup.h"

#include "Common/Poisson.h"
#include "Engine/Event.h"

using namespace Ogre;

#define NUM_TECHNIQUES (((int)InstanceManager::InstancingTechniquesCount) + 1)

class  CPoissonMeshInstance
{
public:

	CPoissonMeshInstance(
		SceneManager* sceneManager,
		String		  meshName,
		String		  materialName,
		Terrain*	  terrain,
		bool		  useInstancing = true,
		InstanceManager::InstancingTechnique technique = InstanceManager::HWInstancingVTF,
		float			   minScale          = 0.01f,
		float			   maxScale          = 10.0f,
		float			   poissonRadius     = 80.0f,
		float			   minHeight         = 60.0f,
		float			   maxHeight         = 700.0f,
		std::vector<float> coverageMap       = std::vector<float>(),
		float			   coverageThreshold = 1.0f
	)
		: mSceneMgr(sceneManager)
		, mMeshName(meshName)
		, mMaterialName(materialName)
		, mTerrain(terrain)
		, mUseInstancing(useInstancing)
		, mInstancingTechnique(technique)
		, mPoissonRadius(poissonRadius)
		, mMinHeight(minHeight)
		, mMaxHeight(maxHeight)
		, mMinScale(minScale)
		, mMaxScale(maxScale)
		, mCoverageMap(coverageMap)
		, mCoverageThreshold(coverageThreshold)
	{
		CEventHandler::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CPoissonMeshInstance::Update, this, std::placeholders::_1));

		mUseMeshVector = false;

		mCurrentManager = 0;

		if (!mCoverageMap.empty() && (sqrt(mCoverageMap.size()) == mTerrain->getSize()))
		{
			mUseCoverageMap = true;
		}
		else
		{
			mUseCoverageMap = false;
		}
		if (mUseCoverageMap)
		{
			std::cout << "Coverage map must be the same size as the terrain size (cov map size: "
				<< sqrt(mCoverageMap.size()) << " terrain size: " << mTerrain->getSize() << ")" << std::endl;
		}

		mSceneMgr->setShadowTextureSelfShadow(false);

		CheckHardwareSupport();

		mInstancedEntities.reserve(NUM_INST_ROW * NUM_INST_COLUMN);

		SetupInstanceManager();

	}

	CPoissonMeshInstance(
		SceneManager* sceneManager,
		std::vector<String> meshNames,
		String		  materialName,
		Terrain*	  terrain,
		bool		  useInstancing = true,
		InstanceManager::InstancingTechnique technique = InstanceManager::HWInstancingVTF,
		float			   minScale			 = 0.01f,
		float			   maxScale			 = 10.0f,
		float			   poissonRadius	 = 80.0f,
		float			   minHeight		 = 60.0f,
		float			   maxHeight		 = 700.0f,
		std::vector<float> coverageMap		 = std::vector<float>(),
		float			   coverageThreshold = 1.0f
	)
		: mSceneMgr(sceneManager)
		, mMeshNames(meshNames)
		, mMaterialName(materialName)
		, mTerrain(terrain)
		, mUseInstancing(useInstancing)
		, mInstancingTechnique(technique)
		, mPoissonRadius(poissonRadius)
		, mMinHeight(minHeight)
		, mMaxHeight(maxHeight)
		, mMinScale(minScale)
		, mMaxScale(maxScale)
		, mCoverageMap(coverageMap)
		, mCoverageThreshold(coverageThreshold)
	{
		CEventHandler::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CPoissonMeshInstance::Update, this, std::placeholders::_1));

		mCurrentManager = 0;

		if (!mMeshNames.empty())
		{
			mMeshName = mMeshNames[0];
			mUseMeshVector = true;
		}
		else
		{
			std::cout << "Mesh vector is empty!" << std::endl;
			return;
		}

		if (!mCoverageMap.empty() && (sqrt(mCoverageMap.size()) == mTerrain->getSize()))
		{
			mUseCoverageMap = true;
		}
		else
		{
			std::cout << "Coverage map must be the same size as the terrain size (cov map size: "
				<< sqrt(mCoverageMap.size()) << " terrain size: " << mTerrain->getSize() << ")" << std::endl;
			mUseCoverageMap = false;
		}

		mSceneMgr->setShadowTextureSelfShadow(false);

		CheckHardwareSupport();

		mInstancedEntities.reserve(NUM_INST_ROW * NUM_INST_COLUMN);

		SetupInstanceManager();

	}

	void PlaceEntities();
	void ClearEntities();

	bool Update(const FrameEvent& evt);



private:
	void CreateInstancedEntities();
	void ClearInstancedEntities();

	void CreateNonInstancedEntities();
	void ClearNonInstancedEntities();

	void SetupInstanceManager();
	void DestroyManagers();
	void DefragmentBatches();
	void TestCapabilities(const RenderSystemCapabilities* caps);

	//The difference between testCapabilities() is that features checked here aren't fatal errors.
	//which means the sample can run (with limited functionality) on those computers
	void CheckHardwareSupport();

	const int						     NUM_INST_ROW    = 100;
	const int						     NUM_INST_COLUMN = 100;
	InstanceManager::InstancingTechnique mInstancingTechnique;
	std::vector<MovableObject*>			 mInstancedEntities;
	std::vector<SceneNode*>				 mNonInstancedEntities;
	InstanceManager*					 mCurrentManager;
	bool								 mTechniqueIsSupported;

	bool mUseInstancing = false;

	SceneManager* mSceneMgr;
	Terrain*	  mTerrain;

	//Parameters
	String mMeshName	 = "";
	String mMaterialName = "";
	std::vector<String> mMeshNames;
	bool mUseMeshVector = false;

	float mPoissonRadius = 80.0f;
	float mMinHeight	 = 60.0f;
	float mMaxHeight	 = 700.0f;
	float mMinScale		 = 0.01f;
	float mMaxScale		 = 10.0f;

	bool  mUseCoverageMap;
	float mCoverageThreshold = 1.0f;
	std::vector<float> mCoverageMap;
};