#include "pch.h"
#include "Game.h"

CGame::CGame() : ApplicationContext("Test")
{
}

CGame::~CGame()
{
}

void CGame::Setup()
{
	ApplicationContext::initApp();
	addInputListener(this);

	mRoot = ApplicationContext::getRoot();

	mWindow = ApplicationContext::getRenderWindow();
	WindowEventUtilities::addWindowEventListener(mWindow, this);

	// Create a new scene manager.
	SceneManager* sceneManager = mRoot->createSceneManager();
	sceneManager->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));

	RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(sceneManager);


	// Create a new camera
	mCamera = new CCamera();
	mCamera->Initialize(sceneManager, mWindow);

	Ogre::ColourValue fadeColour(1, 1, 1, 1);
	mWindow->getViewport(0)->setBackgroundColour(fadeColour);
	//sceneManager->setFog(FOG_EXP, fadeColour, 0.000002);

	mInputMgr = InputManager::getSingletonPtr();
	//mInputMgr->setCamMan(mCamera->getManager());
	mInputMgr->initialise(mWindow);
	mInputMgr->SetCamera(mCamera);
	//mInputMgr->addKeyListener(  this, "ListenerName");
	//mInputMgr->addMouseListener(this, "ListenerName");

	// Add our model to our resources and index it
	ResourceGroupManager::getSingleton().addResourceLocation("media/packs/trees.zip", "Zip");
	ResourceGroupManager::getSingleton().addResourceLocation("media/packs/Sinbad.zip", "Zip");
	ResourceGroupManager::getSingleton().addResourceLocation("media/models/", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/ShadowVolume/", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/RTShaderLib/GLSL", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/Terrain/", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures/house", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/scripts", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures/nvidia", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/skyboxes/sunnytropic", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/imgui/resources", "FileSystem", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);

	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// Load and apply Skybox
	//sceneManager->setSkyBox(true, "skybox", 50000, true);

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


	mTrayManager = new OgreBites::TrayManager("InterfaceName", getRenderWindow());
	sceneManager->addRenderQueueListener(getOverlaySystem());
	mTrayManager->showFrameStats(TrayLocation::TL_BOTTOMLEFT);

	
	// Create an instance of the MyFrameListener Class and add it to the root object
	MyFrameListener* myListener = new MyFrameListener();
	myListener->setCam(mCamera);
	myListener->setWindow(mWindow);
	myListener->setTrayManager(mTrayManager);
	myListener->setApplicationContext(this);
	mRoot->addFrameListener(myListener);

	mTerrain = new CTerrain(sceneManager);
	mTerrain->Initialize(ent->getBoundingBox());

	mModelPlacer = new CModelPlacer(sceneManager, mTerrain);

	mPlantPlacer = new CPlantPlacer(sceneManager, mTerrain);
	mPlantPlacer->Initialize();


	ImGuiOverlay *imguiOverlay = new ImGuiOverlay();
	imguiOverlay->setZOrder(300);
	Ogre::OverlayManager::getSingleton().addOverlay(imguiOverlay); // now owned by overlaymgr
	imguiOverlay->show();
	mImguiListener.reset(new ImGuiInputListener());
	mListenerChain = InputListenerChain({ mImguiListener.get()});
}

void CGame::Update()
{
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
	mApplicationContext->frameStarted(evt);
	mApplicationContext->pollEvents();
	WindowEventUtilities::messagePump();

	ImGuiOverlay::NewFrame(evt);
	ImGui::ShowDemoWindow();

	return true;
}

bool MyFrameListener::frameEnded(const FrameEvent &evt)
{
	return true;
}

bool MyFrameListener::frameRenderingQueued(const FrameEvent &evt)
{
	mCam->FrameRenderingQueued(evt);
	mTrayManager->frameRendered(evt);
	return true;
}