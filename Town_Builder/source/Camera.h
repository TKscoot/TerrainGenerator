#pragma once
#include "Ogre.h"
#include "Bites/OgreCameraMan.h"
#include "InputManager.h"
#include "Bites/OgreCameraMan.h"

using namespace Ogre;
using namespace OgreBites;

class CCamera
{
public:
	void Initialize(SceneManager* sceneManager);
	void Update(Real deltaTime);

	void InjectKeyDown(const OIS::KeyEvent& evt);
	void InjectKeyUp(const OIS::KeyEvent& evt);
	void InjectMouseMove(const OIS::MouseEvent& evt);
	
	bool FrameRenderingQueued(const Ogre::FrameEvent& evt);


	Camera* getCamera() { return mCamera; }
	Vector3 mVelocity;
private:
	Camera* mCamera;
	SceneNode* mCamNode;

	//movement
	bool mGoingForward;
	bool mGoingBack;
	bool mGoingLeft;
	bool mGoingRight;
	bool mGoingUp;
	bool mGoingDown;
	bool mFastMove;

	Ogre::Real mTopSpeed = 350;
	
};