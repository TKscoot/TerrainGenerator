#pragma once

// std
#include <exception>
#include <iostream>

// Ogre
#include <Ogre.h>
#include <OgreRoot.h>
#include <OgreFrameListener.h>
#include <OgreConfigDialog.h>

// GUI
#include "Overlay/OgreImGuiOverlay.h"
#include "Overlay/OgreOverlaySystem.h"
#include "Overlay/OgreOverlayManager.h"
#include "Overlay/imgui.h"

// Bites
#include "Bites/OgreApplicationContext.h"
#include "Bites/OgreWindowEventUtilities.h"
#include "Bites/OgreImGuiInputListener.h"

// Game Logic
#include "InputManager.h"
#include "Camera.h"
#include "Terrain.h"
#include "ModelPlacer.h"
#include "PlantPlacer.h"
#include "Event.h"

using namespace Ogre;

class CGame : public OgreBites::InputListener, public OgreBites::ApplicationContext, public OgreBites::WindowEventListener
{
public:
	CGame();
	~CGame();

	// Input Callbacks TODO: redo InputManager to take function pointers/lambdas
	virtual void frameRendered(		const Ogre::FrameEvent& evt) {}
	virtual bool keyPressed(		const KeyboardEvent& evt)	 
	{ 
		if (evt.keysym.sym == 'm')
		{
			mShowCursor = !mShowCursor;
			ShowCursor(mShowCursor);
		}

		mListenerChain.keyPressed(evt);  
		mCamera->InjectKeyDown(evt); return true; 
	}
	virtual bool keyReleased(		const KeyboardEvent& evt)	 { mListenerChain.keyReleased(evt); mCamera->InjectKeyUp(evt); return true; }
	virtual bool touchMoved(		const TouchFingerEvent& evt) { return true; }
	virtual bool touchPressed(		const TouchFingerEvent& evt) { return true; }
	virtual bool touchReleased(		const TouchFingerEvent& evt) { return true; }
	virtual bool mouseMoved(		const MouseMotionEvent& evt) { mListenerChain.mouseMoved(evt); mCamera->InjectMouseMove(evt); return true; }
	virtual bool mouseWheelRolled(	const MouseWheelEvent& evt)  { mListenerChain.mouseWheelRolled(evt); return true; }
	virtual bool mousePressed(		const MouseButtonEvent& evt) { mListenerChain.mousePressed(evt); return true; }
	virtual bool mouseReleased(		const MouseButtonEvent& evt) { mListenerChain.mouseReleased(evt); return true; }
	virtual bool textInput(			const TextInputEvent& evt)   { return true; }

	// Window callbacks
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
	bool Update(const FrameEvent &evt);
	void Render();

	// Getter & Setter
	InputManager* getInputMgr() { return mInputMgr; }
	Root* getRoot() { return mRoot; }
	RenderWindow* getWindow() { return mWindow; }

private:

	// Ogre stuff
	Root*					   mRoot;
	RenderWindow*			   mWindow;
	RTShader::ShaderGenerator* mShaderGenerator;

	// Object Instances
	InputManager* mInputMgr;
	CCamera*      mCamera;

	CTerrain*     mTerrain;
	CModelPlacer* mModelPlacer;
	CPlantPlacer* mPlantPlacer;

	// Input Listeners
	std::unique_ptr<ImGuiInputListener> mImguiListener;
	InputListenerChain mListenerChain;

	// Logic stuff
	bool mShowCursor = true;
};

