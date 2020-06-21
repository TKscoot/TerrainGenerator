#pragma once
#include <Ogre.h>
#include <random>

#include "Terrain/Terrain.h"
#include "Common/Poisson.h"

using namespace Ogre;

#define NUM_TECHNIQUES (((int)InstanceManager::InstancingTechniquesCount) + 1)

class  CPoissonMeshInstance
{
public:

	CPoissonMeshInstance(
		SceneManager* sceneManager, 
		String meshName, 
		String materialName, 
		CTerrain* terrain, 
		InstanceManager::InstancingTechnique technique = InstanceManager::HWInstancingVTF)
		: mSceneMgr(sceneManager)
		, mMeshName(meshName)
		, mMaterialName(materialName)
		, mTerrain(terrain)
		, mInstancingTechnique(technique)
	{
		CEventHandler::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CPoissonMeshInstance::Update, this, std::placeholders::_1));

		mCurrentManager = 0;
		
		CheckHardwareSupport();

		mEntities.reserve(NUM_INST_ROW * NUM_INST_COLUMN);


		SetupInstanceManager();

	}

	//bool frameRenderingQueued(const FrameEvent& evt);

	//bool keyPressed(const KeyboardEvent& evt);

	bool Update(const FrameEvent& evt)
	{
		if (ImGui::CollapsingHeader("PoissonInstancing", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::BeginChild("PoissonInstancing", ImVec2(0, 135), true);
			ImGui::SliderFloat("Radius",	 &mPoissonRadius, 20.0f, 500.0f);
			ImGui::SliderFloat("Min height", &mMinHeight	, 0.0f, 1000.0f);
			ImGui::SliderFloat("Max height", &mMaxHeight	, 0.0f, 1000.0f);
			ImGui::Spacing();
			if (ImGui::Button("Place trees!"))
			{
				ClearScene();
				CreateInstancedEntities();
			}
			ImGui::EndChild();
		}

		ImGui::End();

		return true;
	}





private:
	void SetupInstanceManager();

	void CreateInstancedEntities();

	void ClearScene();

	void DestroyManagers();

	void CleanupContent();

	void DefragmentBatches();

	void TestCapabilities(const RenderSystemCapabilities* caps);

	//The difference between testCapabilities() is that features checked here aren't fatal errors.
	//which means the sample can run (with limited functionality) on those computers
	void CheckHardwareSupport();

	const int						     NUM_INST_ROW    = 80;
	const int						     NUM_INST_COLUMN = 80;
	InstanceManager::InstancingTechnique mInstancingTechnique;
	std::vector<MovableObject*>			 mEntities;
	InstanceManager*					 mCurrentManager;
	bool								 mTechniqueIsSupported;

	SceneManager* mSceneMgr;
	CTerrain*	  mTerrain;

	//Parameters
	String mMeshName	 = "";
	String mMaterialName = "";

	float mPoissonRadius = 80.0f;
	float mMinHeight	 = 50.0f;
	float mMaxHeight	 = 450.0f;
};