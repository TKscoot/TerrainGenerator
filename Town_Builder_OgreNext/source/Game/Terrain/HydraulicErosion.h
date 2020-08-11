#pragma once
#include <chrono>
#include <Ogre.h>
#include "Engine/Event.h"
#include "imgui.h"

using namespace Ogre;

class CHydraulicErosion
{
public:
	CHydraulicErosion(int size, std::function<void(int)> erodeFunction)
		: mSize(size)
		, mErodeFunction(erodeFunction)

	{
		CEventHandler::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CHydraulicErosion::Update, this, std::placeholders::_1));
	}

	bool Update(const FrameEvent& evt);
	void Calculate(std::vector<float> &map);

	void ResetParameters()
	{
		mIterations			 = 1000;

		mMaxDropletLifetime  = 30;
		mInertia		     = 0.05f;
		mInitialSpeed		 = 1.0f;
		mInitialWater		 = 1.0f;
		mInitialSediment	 = 0.0f;
		mMinSlope			 = 0.1f;
		mMaxSedimentCapacity = 1.0f;
		mDeposition			 = 0.8f;
		mErosion			 = 0.01f;
		mGravity			 = 4.0f;
		mEvaporationSpeed	 = 0.01f;
	}

private:

	int mSize = 0;
	std::function<void(int)> mErodeFunction;
	

	// Erosion parameters
	int   mIterations          = 1000;

	int   mMaxDropletLifetime  = 30;
	float mInertia			   = 0.05f;
	float mInitialSpeed		   = 1.0f;
	float mInitialWater		   = 1.0f;
	float mInitialSediment	   = 0.0f;
	float mMinSlope			   = 0.1f;
	float mMaxSedimentCapacity = 1.0f;
	float mDeposition		   = 0.8f;
	float mErosion		       = 0.01f;
	float mGravity			   = 4.0f;
	float mEvaporationSpeed	   = 0.01f;

	// current vars (members so i can use them with ImGui)
	float mSpeed    = 1.0f;
	float mWater    = 1.0f;
	float mSediment = 0.0f;

};