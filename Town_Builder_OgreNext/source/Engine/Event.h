#pragma once
#include <OgreFrameListener.h>
#include <Bites/OgreApplicationContext.h>
#include <Ogre.h>
#include <iostream>
#include <imgui.h>

using namespace Ogre;

class CEventHandler : public FrameListener
{
public:
	CEventHandler()
	{
		Root::getSingleton().addFrameListener(this);
	}

	void Destroy()
	{
		delete mInstance;
		mInstance = nullptr;
	}

	static CEventHandler* GetSingletonPtr()
	{
		if (mInstance == nullptr)
		{
			mInstance = new CEventHandler();
		}

		return mInstance;
	}
	
	//
	// Adds Frame Callback when a new frame has started (Equivalent to unitys Update() function)
	// @arg1 Pass your method function in here. Use std::bind(&CMyClass::MyMethod, this, std::placeholders::_1)
	//		 Your method needs to return a bool and needs (const FrameEvent&) as argument
	//		 Alternatively use Lambdas
	//
	void AddFrameStartedCallback(std::function<bool(const FrameEvent&)> callback)
	{
		mFrameStartedCallbacks.push_back(callback);
	}

	//
	// Adds Frame Callback when a frame has ended
	// @arg1 Pass your method function in here. Use std::bind(&CMyClass::MyMethod, this, std::placeholders::_1)
	//		 Your method needs to return a bool and needs (const FrameEvent&) as argument
	//		 Alternatively use Lambdas
	//
	void AddFrameEndedCallback(std::function<bool(const FrameEvent&)> callback)
	{
		mFrameEndedCallbacks.push_back(callback);
	}

	//
	// Adds Frame Callback when a new frame has been queued
	// @arg1 Pass your method function in here. Use std::bind(&CMyClass::MyMethod, this, std::placeholders::_1)
	//		 Your method needs to return a bool and needs (const FrameEvent&) as argument
	//		 Alternatively use Lambdas
	//
	void AddFrameRenderingQueuedCallback(std::function<bool(const FrameEvent&)> callback)
	{
		mFrameRenderingQueuedCallbacks.push_back(callback);
	}


private:

	// Methods

	bool frameStarted(const FrameEvent &evt);
	bool frameEnded(const FrameEvent &evt);
	bool frameRenderingQueued(const FrameEvent &evt);

	// Variables
	std::vector<std::function<bool(const FrameEvent&)>> mFrameStartedCallbacks;
	std::vector<std::function<bool(const FrameEvent&)>> mFrameEndedCallbacks;
	std::vector<std::function<bool(const FrameEvent&)>> mFrameRenderingQueuedCallbacks;

	static CEventHandler* mInstance;

};