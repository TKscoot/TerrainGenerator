#include "Camera.h"

void CCamera::Initialize(SceneManager* sceneManager, RenderWindow* window)
{
	mWindow = window;

	mCamera = sceneManager->createCamera("Camera");
	mCamera->setNearClipDistance(1);

	mCamNode = sceneManager->getRootSceneNode()->createChildSceneNode("camera", Ogre::Vector3(0, 50000, 15));
	mCamNode->attachObject(mCamera);
	mCamNode->setFixedYawAxis(true);
	mCamNode->setPosition(Vector3(0, 0, 0));
	
	// Using the camera create a viewport and set the background color to black
	Viewport* viewport = window->addViewport(mCamera);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));

	// Use the viewport to set the aspect ratio of the camera
	mCamera->setAspectRatio(Real(viewport->getActualWidth()) /
		Real(viewport->getActualHeight()));

	// Add Update Callback
	CEvent::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CCamera::Update, this, std::placeholders::_1));
}

bool CCamera::Update(const FrameEvent& evt)
{
	ImGui::Begin("Camera");
	ImGui::Checkbox("Freelook camera", &mFreeLook);
	ImGui::End();

	if (!mFreeLook)
	{
		mCamNode->setPosition(Vector3(-12000, 9000, 11000));
		mCamNode->lookAt(Vector3(0, 0, 0), Node::TransformSpace::TS_WORLD);
	}
	else
	{
		int x = mWindow->getWidth() * 0.5f;
		int y = mWindow->getHeight() * 0.5f;
	}

	return true;
}

void CCamera::InjectKeyDown(const KeyboardEvent& evt)
{

	if (mFreeLook)
	{
		if		(evt.keysym.sym == 'w')	        mGoingForward = true;
		else if (evt.keysym.sym == 's')	        mGoingBack	  = true;
		else if (evt.keysym.sym == 'a')	        mGoingLeft	  = true;
		else if (evt.keysym.sym == 'd')	        mGoingRight   = true;
		else if (evt.keysym.sym == 'e')	        mGoingUp	  = true;
		else if (evt.keysym.sym == 'q')	        mGoingDown	  = true;
		else if (evt.keysym.sym == SDLK_LSHIFT)	mFastMove	  = true;
	}
}

void CCamera::InjectKeyUp(const KeyboardEvent& evt)
{
	if (mFreeLook)
	{
		if      (evt.keysym.sym == 'w')			mGoingForward = false;
		else if (evt.keysym.sym == 's')	        mGoingBack	  = false;
		else if (evt.keysym.sym == 'a')	        mGoingLeft    = false;
		else if (evt.keysym.sym == 'd')	        mGoingRight   = false;
		else if (evt.keysym.sym == 'e')	        mGoingUp	  = false;
		else if (evt.keysym.sym == 'q')	        mGoingDown	  = false;
		else if (evt.keysym.sym == SDLK_LSHIFT)	mFastMove	  = false;
	}
}

void CCamera::InjectMouseMove(const MouseMotionEvent& evt)
{
	if (mFreeLook)
	{
		mCamNode->pitch(Ogre::Degree(-evt.yrel * 0.15f), Node::TransformSpace::TS_LOCAL);
		mCamNode->yaw(  Ogre::Degree(-evt.xrel * 0.15f), Node::TransformSpace::TS_WORLD);
	}

}

bool CCamera::FrameRenderingQueued(const Ogre::FrameEvent& evt)
{

	Quaternion orientation = mCamNode->_getDerivedOrientation();
	Vector3    up		   = orientation * Vector3::UNIT_Y;
	Vector3    forward     = orientation * Vector3::UNIT_Z;
	Vector3    right	   = orientation * Vector3::UNIT_X;


	// build our acceleration vector based on keyboard input composite
	Ogre::Vector3      accel  = Ogre::Vector3::ZERO;
	if (mGoingForward) accel -= forward;
	if (mGoingBack)    accel += forward;
	if (mGoingRight)   accel += right;
	if (mGoingLeft)    accel -= right;
	if (mGoingUp)      accel += up;
	if (mGoingDown)    accel -= up;

	// if accelerating, try to reach top speed in a certain time
	Ogre::Real topSpeed = mFastMove ? mTopSpeed * 10 : mTopSpeed;
	if (accel.squaredLength() != 0)
	{
		accel.normalise();
		mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
	}
	// if not accelerating, try to stop in a certain time
	else mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;

	Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();

	// keep camera velocity below top speed and above epsilon
	if (mVelocity.squaredLength() > topSpeed * topSpeed)
	{
		mVelocity.normalise();
		mVelocity *= topSpeed;
	}
	else if (mVelocity.squaredLength() < tooSmall * tooSmall)
		mVelocity = Ogre::Vector3::ZERO;

	if (mVelocity != Ogre::Vector3::ZERO)
	{
		if (mFreeLook)
		{
			mCamNode->translate(mVelocity * evt.timeSinceLastFrame);
		}

	}

	return true;
}
