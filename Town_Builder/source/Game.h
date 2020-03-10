#pragma once
#include <exception>
#include <iostream>
#include "Ogre.h"
#include "OgreFrameListener.h"
#include "Bites/OgreInput.h"
#include "InputManager.h"
#include "Camera.h"
#include "Terrain.h"
#include "Bites/OgreTrays.h"
#include "ModelPlacer.h"
#include "PlantPlacer.h"

using namespace Ogre;

class CGame : public OIS::KeyListener, OIS::MouseListener
{
public:
	CGame();
	~CGame();

	bool keyPressed(const OIS::KeyEvent &arg) 
	{
		mCamera->InjectKeyDown(arg);
		mTerrain->InjectKeyDown(arg);
		mModelPlacer->InjectKeyDown(arg);

		if (arg.key == OIS::KC_ESCAPE)
		{
			exit(0);
		}

		return false; 
	}
	bool keyReleased(const OIS::KeyEvent &arg) 
	{ 
		mCamera->InjectKeyUp(arg);
		mTerrain->InjectKeyUp(arg);

		return false; 
	}

	bool mouseMoved(const OIS::MouseEvent &arg) 
	{ 
		mCamera->InjectMouseMove(arg); 
		return true; 
	}
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id) 
	{ 
		//mCamera->getManager()->injectMouseDown(arg, id);
		mModelPlacer->InjectMousePress(arg, id);
		return true; 
	}
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) 
	{ 
		//mCamera->getManager()->injectMouseUp(arg, id);

		return false; 
	}


	void Setup();
	void Update();
	void Render();

	// Getter & Setter
	InputManager* getInputMgr() { return mInputMgr; }
	Root* getRoot() { return mRoot; }
	RenderWindow* getWindow() { return mWindow; }

private:
	InputManager* mInputMgr;
	CCamera* mCamera;

	Root* mRoot;
	RenderWindow* mWindow;
	OIS::Mouse* mMouse;

	CTerrain* mTerrain;
	CModelPlacer* mModelPlacer;
	CPlantPlacer* mPlantPlacer;
};


class MyFrameListener : public FrameListener
{
public:
	bool frameStarted(const FrameEvent &evt);
	bool frameEnded(const FrameEvent &evt);
	bool frameRenderingQueued(const FrameEvent &evt);
	void setCam(CCamera* cam) { mCam = cam; }
	void setRoot(Root* root) { mRoot = root; }

	CCamera* mCam;
	Root* mRoot;

};

