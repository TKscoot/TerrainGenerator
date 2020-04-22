#include "InputManager.h"

InputManager *InputManager::mInputManager;

InputManager::InputManager(void)
{
}

InputManager::~InputManager(void)
{
}

void InputManager::initialise(Ogre::RenderWindow *renderWindow)
{

}

InputManager* InputManager::getSingletonPtr(void)
{
	if (!mInputManager)
	{
		mInputManager = new InputManager();
	}
	return mInputManager;
}