#include "pch.h"
#include "Camera.h"

void CCamera::Initialize(SceneManager* sceneManager)
{
	mCamera = sceneManager->createCamera("Camera");
	mCamera->setPosition(Ogre::Vector3(0, 0, 0));
	mCamera->lookAt(Ogre::Vector3(0, 0, 0));
	mCamera->setNearClipDistance(1);


	mCamNode = sceneManager->getRootSceneNode()->createChildSceneNode("camera", Ogre::Vector3(0, 50000, 15));
	mCamNode->attachObject(mCamera);
	mCamNode->setFixedYawAxis(true);
	mCamNode->setPosition(Vector3(0, 0, 0));
	
	// Using the camera create a viewport and set the background color to black
	Viewport* viewport = Root::getSingletonPtr()->getAutoCreatedWindow()->addViewport(mCamera);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));

	// Use the viewport to set the aspect ratio of the camera
	mCamera->setAspectRatio(Real(viewport->getActualWidth()) /
		Real(viewport->getActualHeight()));

}

void CCamera::Update(Real deltaTime)
{

}

void CCamera::InjectKeyDown(const OIS::KeyEvent& evt)
{

	if		(evt.key == OIS::KC_W || evt.key == OIS::KC_UP)		mGoingForward = true;
	else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN)	mGoingBack = true;
	else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT)	mGoingLeft = true;
	else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT)	mGoingRight = true;
	else if (evt.key == OIS::KC_Q || evt.key == OIS::KC_PGUP)	mGoingUp = true;
	else if (evt.key == OIS::KC_E || evt.key == OIS::KC_PGDOWN) mGoingDown = true;
	else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
}

void CCamera::InjectKeyUp(const OIS::KeyEvent& evt)
{

	if      (evt.key == OIS::KC_W || evt.key == OIS::KC_UP)		mGoingForward = false;
	else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN)	mGoingBack = false;
	else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT)	mGoingLeft = false;
	else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT)	mGoingRight = false;
	else if (evt.key == OIS::KC_Q || evt.key == OIS::KC_PGUP)	mGoingUp = false;
	else if (evt.key == OIS::KC_E || evt.key == OIS::KC_PGDOWN) mGoingDown = false;
	else if (evt.key == OIS::KC_LSHIFT) mFastMove = false;
}

void CCamera::InjectMouseMove(const OIS::MouseEvent & evt)
{
	mCamNode->pitch(Ogre::Degree(-evt.state.Y.rel * 0.15f), Node::TransformSpace::TS_LOCAL);
	mCamNode->yaw(Ogre::Degree(-evt.state.X.rel * 0.15f), Node::TransformSpace::TS_WORLD);

}

bool CCamera::FrameRenderingQueued(const Ogre::FrameEvent& evt)
{

	Quaternion orientation = mCamNode->_getDerivedOrientation();
	Vector3 up		= orientation * Vector3::UNIT_Y;
	Vector3 forward = orientation * Vector3::UNIT_Z;
	Vector3 right	= orientation * Vector3::UNIT_X;


	// build our acceleration vector based on keyboard input composite
	Ogre::Vector3 accel = Ogre::Vector3::ZERO;
	if (mGoingForward) accel -= forward;
	if (mGoingBack) accel	 += forward;
	if (mGoingRight) accel	 += right;
	if (mGoingLeft) accel	 -= right;
	if (mGoingUp) accel		 += up;
	if (mGoingDown) accel	 -= up;

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
		//mCamera->move(mVelocity * evt.timeSinceLastFrame);
		mCamNode->translate(mVelocity * evt.timeSinceLastFrame);

	}

	return true;
}
