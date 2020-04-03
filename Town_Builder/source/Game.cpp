#include "pch.h"
#include "Game.h"

CGame::CGame()
{

}

CGame::~CGame()
{
}

void CGame::Setup()
{

	mRoot = new Root();
	// Configures the application
	if (!mRoot->restoreConfig())
	{
		mRoot->showConfigDialog(nullptr);
		mRoot->saveConfig();
	}

	// Create Render Window
	mWindow = mRoot->initialise(true, "test");
	mWindow->resize(1280, 720);

	// Create a new scene manager.
	SceneManager* sceneManager = mRoot->createSceneManager();
	sceneManager->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));


	if (Ogre::RTShader::ShaderGenerator::initialize())
	{
		// Grab the shader generator pointer.
		mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
		// Add the shader libs resource location. a sample shader lib can be found in Samples\Media\RTShaderLib
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/RTShaderLib", "FileSystem");
		// Set shader cache path.
		mShaderGenerator->setShaderCachePath("media/RTShaderLib/cache");
		// Set the scene manager.
		mShaderGenerator->addSceneManager(sceneManager);
	}


	// Create a new camera
	mCamera = new CCamera();
	mCamera->Initialize(sceneManager);

	Ogre::ColourValue fadeColour(1, 1, 1, 1);
	mWindow->getViewport(0)->setBackgroundColour(fadeColour);
	//sceneManager->setFog(FOG_EXP, fadeColour, 0.000002);

	mInputMgr = InputManager::getSingletonPtr();
	//mInputMgr->setCamMan(mCamera->getManager());
	mInputMgr->initialise(mWindow);
	mInputMgr->addKeyListener(  this, "ListenerName");
	mInputMgr->addMouseListener(this, "ListenerName");

	// Add our model to our resources and index it
	ResourceGroupManager::getSingleton().addResourceLocation("media/packs/trees.zip", "Zip");
	ResourceGroupManager::getSingleton().addResourceLocation("media/packs/Sinbad.zip", "Zip");
	ResourceGroupManager::getSingleton().addResourceLocation("media/models/", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures/house", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/scripts", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures/nvidia", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/skyboxes/sunnytropic", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/imgui/resources", "FileSystem", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);

	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// Load and apply Skybox
	sceneManager->setSkyBox(true, "skybox", 50000, true);

	// Create Light
	Light* light1 = sceneManager->createLight("Light1");
	light1->setType(Ogre::Light::LT_POINT);
	// Set Light Color
	light1->setDiffuseColour(1.0f, 1.0f, 1.0f);
	// Set Light Reflective Color
	light1->setSpecularColour(1.0f, 0.0f, 0.0f);
	// Set Light (Range, Brightness, Fade Speed, Rapid Fade Speed)
	light1->setAttenuation(10, 0.5, 0.045, 0.0);


	// Create light Entity
	Entity* lightEnt = sceneManager->createEntity("LightEntity", "sphere.mesh");
	SceneNode* lightNode = sceneManager->createSceneNode("LightNode");
	lightNode->attachObject(lightEnt);
	lightNode->attachObject(light1);
	lightNode->setScale(0.01f, 0.01f, 0.01f);
	lightNode->setPosition(0, 4, 10);
	
	sceneManager->getRootSceneNode()->addChild(lightNode);
	// Create an instance of our model and add it to the scene
	Entity* ent = sceneManager->createEntity("house.mesh");
	SceneNode* entNode = sceneManager->createSceneNode("Character");
	entNode->attachObject(ent);
	sceneManager->getRootSceneNode()->addChild(entNode);
	entNode->setPosition(100, 1000, -450);
	entNode->scale(Vector3(20, 20, 20));
	entNode->showBoundingBox(true);
	
	// Create an instance of the MyFrameListener Class and add it to the root object
	MyFrameListener* myListener = new MyFrameListener();
	myListener->setCam(mCamera);
	myListener->setWindow(mWindow);
	mRoot->addFrameListener(myListener);

	mTerrain = new CTerrain(sceneManager);
	mTerrain->Initialize(ent->getBoundingBox());

	mModelPlacer = new CModelPlacer(sceneManager, mTerrain);

	mPlantPlacer = new CPlantPlacer(sceneManager, mTerrain);
	mPlantPlacer->Initialize();

	ImGui::CreateContext();
	ImguiManager::createSingleton();
	ImguiManager::getSingleton().init(sceneManager);
}

void CGame::Update()
{
	mInputMgr->getKeyboard()->capture();
	mInputMgr->getMouse()->capture();

	mTerrain->Update();
	mModelPlacer->Update();
	mPlantPlacer->Update();

	mWindow->update();
}

void CGame::Render()
{


	mRoot->renderOneFrame();
}

///
/// Frame Listener stuff
///

bool MyFrameListener::frameStarted(const FrameEvent &evt)
{
	static float t = 0;
	t += evt.timeSinceLastFrame;

	if (t >= 5.0f)
	{
		float fps = 1 / evt.timeSinceLastFrame;
		std::cout << "FPS: " << fps << std::endl;
		t = 0;
	}

	Ogre::ImguiManager::getSingleton().newFrame(
		evt.timeSinceLastFrame,
		Ogre::Rect(0, 0, mWindow->getWidth(), mWindow->getHeight()));

	ImGui::ShowTestWindow();

	return true;
}

bool MyFrameListener::frameEnded(const FrameEvent &evt)
{
	return true;
}

bool MyFrameListener::frameRenderingQueued(const FrameEvent &evt)
{
	mCam->FrameRenderingQueued(evt);
	return true;
}