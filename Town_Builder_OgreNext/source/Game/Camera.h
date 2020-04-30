#pragma once
#include "Ogre.h"
#include "Bites/OgreInput.h"
#include "Engine/Event.h"
#include "imgui.h"

#include <Windows.h>

using namespace Ogre;
using namespace OgreBites;

class CCamera
{
public:
	void Initialize(SceneManager* sceneManager, RenderWindow* window);
	bool Update(const FrameEvent& evt);

	void InjectKeyDown(const KeyboardEvent& evt);
	void InjectKeyUp(const KeyboardEvent& evt);
	void InjectMouseMove(const MouseMotionEvent& evt);
	
	bool FrameRenderingQueued(const Ogre::FrameEvent& evt);


	Camera* getCamera() { return mCamera; }
	Ogre::Vector3 mVelocity;
private:
	Camera*		  mCamera;
	SceneNode*	  mCamNode;
	RenderWindow* mWindow;

	//movement
	bool mGoingForward;
	bool mGoingBack;
	bool mGoingLeft;
	bool mGoingRight;
	bool mGoingUp;
	bool mGoingDown;
	bool mFastMove;

	Ogre::Real mTopSpeed = 350;

	bool mFreeLook = false;
	
};