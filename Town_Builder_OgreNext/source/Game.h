#pragma once
#include <exception>
#include <iostream>
#include <Ogre.h>
#include <OgreRoot.h>
#include <OgreFrameListener.h>
#include <OgreConfigDialog.h>
#include "Bites/OgreApplicationContext.h"
#include "Bites/OgreWindowEventUtilities.h"
#include "Overlay/OgreImGuiOverlay.h"
#include "Overlay/OgreOverlaySystem.h"
#include "Overlay/OgreOverlayManager.h"
#include "Bites/OgreTrays.h"
#include "Overlay/imgui.h"
#include "Bites/OgreImGuiInputListener.h"
#include "InputManager.h"
#include "Camera.h"
#include "Terrain.h"
#include "ModelPlacer.h"
#include "PlantPlacer.h"
#include "SdkSample.h"

using namespace Ogre;

class CGame : public OgreBites::InputListener, public OgreBites::ApplicationContext, public OgreBites::WindowEventListener, public OgreBites::TrayListener
{
public:
	CGame();
	~CGame();

	virtual void frameRendered(		const Ogre::FrameEvent& evt) {}
	virtual bool keyPressed(const KeyboardEvent& evt) { mListenerChain.keyPressed(evt);  mCamera->InjectKeyDown(evt); return true; }
	virtual bool keyReleased(		const KeyboardEvent& evt)	 { mListenerChain.keyReleased(evt); mCamera->InjectKeyUp(evt); return true; }
	virtual bool touchMoved(		const TouchFingerEvent& evt) { return true; }
	virtual bool touchPressed(		const TouchFingerEvent& evt) { return true; }
	virtual bool touchReleased(		const TouchFingerEvent& evt) { return true; }
	virtual bool mouseMoved(		const MouseMotionEvent& evt) { mListenerChain.mouseMoved(evt); mCamera->InjectMouseMove(evt); return true; }
	virtual bool mouseWheelRolled(	const MouseWheelEvent& evt)  { mListenerChain.mouseWheelRolled(evt); return true; }
	virtual bool mousePressed(		const MouseButtonEvent& evt) { mListenerChain.mousePressed(evt); return true; }
	virtual bool mouseReleased(		const MouseButtonEvent& evt) { mListenerChain.mouseReleased(evt); return true; }
	virtual bool textInput(			const TextInputEvent& evt)   { return true; }

	bool windowClosing(Ogre::RenderWindow* rw) 
	{ 
		exit(0);
		return true; 
	}

	void windowClosed(Ogre::RenderWindow* rw)
	{
		exit(0);
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
	RTShader::ShaderGenerator* mShaderGenerator;

	CTerrain* mTerrain;
	CModelPlacer* mModelPlacer;
	CPlantPlacer* mPlantPlacer;

	std::unique_ptr<ImGuiInputListener> mImguiListener;
	InputListenerChain mListenerChain;
	OgreBites::TrayManager *mTrayManager;
};


class MyFrameListener : public FrameListener
{
public:
	bool frameStarted(const FrameEvent &evt);
	bool frameEnded(const FrameEvent &evt);
	bool frameRenderingQueued(const FrameEvent &evt);
	void setCam(CCamera* cam) { mCam = cam; }
	void setRoot(Root* root) { mRoot = root; }
	void setWindow(RenderWindow* window) { mWindow = window; }
	void setTrayManager(OgreBites::TrayManager* trayManager) { mTrayManager = trayManager; }
	void setApplicationContext(ApplicationContext* applicationContext) { mApplicationContext = applicationContext; }
	
	CCamera*				   mCam;
	Root*					   mRoot;
	RenderWindow*			   mWindow;
	OgreBites::TrayManager*    mTrayManager;
	ApplicationContext*		   mApplicationContext;
};
