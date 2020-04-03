#pragma once
#include "Bites/OgreInput.h"
#include <OgreRenderWindow.h>
#include "Camera.h"

using namespace Ogre;
using namespace OgreBites;

class InputManager : public OgreBites::InputListener
{
public:
	virtual ~InputManager(void);

	void initialise(Ogre::RenderWindow *renderWindow);
	//void capture(void);

	void frameRendered(	  const Ogre::FrameEvent& evt) {}
	bool keyPressed(	  const KeyboardEvent& evt) 
	{ 
		mCam->InjectKeyDown(evt);
		return false; 
	}
	bool keyReleased(	  const KeyboardEvent& evt)    
	{ 
		mCam->InjectKeyUp(evt);
		return false; 
	}
	bool touchMoved(	  const TouchFingerEvent& evt) 
	{ 
		return false; 
	}
	bool touchPressed(	  const TouchFingerEvent& evt) 
	{ 
		return false; }
	bool touchReleased(	  const TouchFingerEvent& evt) 
	{ 
		return false; }
	bool mouseMoved(	  const MouseMotionEvent& evt) 
	{ 
		return false; }
	bool mouseWheelRolled(const MouseWheelEvent& evt) 
	{ 
		return false; }
	bool mousePressed(	  const MouseButtonEvent& evt) 
	{ 
		return false; }
	bool mouseReleased(	  const MouseButtonEvent& evt) 
	{ 
		return false; }
	bool textInput(		  const TextInputEvent& evt) 
	{ 
		return false; }

	//void removeAllListeners(void);
	//void removeAllKeyListeners(void);
	//void removeAllMouseListeners(void);
	//void removeAllJoystickListeners(void);


	static InputManager* getSingletonPtr(void);

	// Getter & Setter

	void SetCamera(CCamera* cam) { mCam = cam; }

private:
	InputManager(void);
	InputManager(const InputManager&) {}
	//InputManager & operator = (const InputManager&);
	CCamera* mCam;

	static InputManager *mInputManager;
};