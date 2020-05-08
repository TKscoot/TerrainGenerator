#include "Game.h"
template <typename R, typename ... Types>
constexpr std::integral_constant<unsigned, sizeof ...(Types)> getArgumentCount(R(*f)(Types ...))
{
	return std::integral_constant<unsigned, sizeof ...(Types)>{};
}
CGame::CGame() : ApplicationContext("Town Builder")
{
}

CGame::~CGame()
{
}

void CGame::Setup()
{
	// Init Ogre
	ApplicationContext::initApp();
	addInputListener(this);

	// Set Root and Window pointers
	mRoot = ApplicationContext::getRoot();
	mWindow = ApplicationContext::getRenderWindow();
	mWindow->setVSyncEnabled(false);
	WindowEventUtilities::addWindowEventListener(mWindow, this);


	// Create a new scene manager.
	SceneManager* sceneManager = mRoot->createSceneManager();
	sceneManager->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));
	
	//Create ShaderGenerator instance
	RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(sceneManager);
	RTShader::ShaderGenerator::getSingleton().initialize();

	// Gui listener
	sceneManager->addRenderQueueListener(getOverlaySystem());

	// Add Frame events using methods or lambdas
	CEvent::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&CGame::Update, this, std::placeholders::_1));
	CEvent::GetSingletonPtr()->AddFrameEndedCallback([](const FrameEvent& evt) -> bool 
	{
		ImGui::EndFrame();
		return true;
	});
	CEvent::GetSingletonPtr()->AddFrameRenderingQueuedCallback([&cam = mCamera](const FrameEvent& evt) -> bool
	{
		cam->FrameRenderingQueued(evt);
		return true;
	});

	// Create a new camera
	mCamera = new CCamera();
	mCamera->Initialize(sceneManager, mWindow);

	// Clear viewport
	Ogre::ColourValue fadeColour(1, 1, 1, 1);
	mWindow->getViewport(0)->setBackgroundColour(fadeColour);

	// Create InputManager instance and initialise it
	mInputMgr = InputManager::getSingletonPtr();
	mInputMgr->initialise(mWindow);
	mInputMgr->SetCamera(mCamera);

	// Add resources and load/initialise them
	ResourceGroupManager::getSingleton().addResourceLocation("media/packs/Sinbad.zip", "Zip");
	ResourceGroupManager::getSingleton().addResourceLocation("media/models/", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/ShadowVolume/", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/RTShaderLib/GLSL", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/Terrain/", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures/skyboxes/sunnytropic", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures/house", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/scripts", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/materials/textures/nvidia", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/skyboxes/sunnytropic", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("media/imgui/resources", "FileSystem", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);

	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();


	MaterialPtr casterMat = MaterialManager::getSingletonPtr()->getByName("Ogre/shadow/depth/caster");

	sceneManager->setShadowFarDistance(3000);

	// 3 textures per directional light (see http://www.stevestreeting.com/2008/08/2 ... -are-cool/)
	sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
	sceneManager->setShadowTextureSettings(2048, 3, Ogre::PF_FLOAT32_R);  // Uses three 512x512 shadow textures
   // You can also do a more detailed setup, for example:

	sceneManager->setShadowTextureCount(3);
	sceneManager->setShadowTextureSelfShadow(true);
	sceneManager->setShadowCasterRenderBackFaces(true);  // Good for shadow casters like tree leaves

	// Set up caster material - this is just a standard (RTSS compatible) depth/shadow map caster
	sceneManager->setShadowTextureCasterMaterial(casterMat);

	// shadow camera setup
	Ogre::PSSMShadowCameraSetup* pssmSetup = new Ogre::PSSMShadowCameraSetup();
	if (mPssmSetup)
		delete mPssmSetup;
	mPssmSetup = pssmSetup;

	mPssmSetup->calculateSplitPoints(3, 0.1, sceneManager->getShadowFarDistance());    // Calculate 3 split points (PSSM 3)
			// Increase near distance when experiencing artifacts
	mPssmSetup->setSplitPadding(0.1);
	mPssmSetup->setOptimalAdjustFactor(0, 2);
	mPssmSetup->setOptimalAdjustFactor(1, 1);
	mPssmSetup->setOptimalAdjustFactor(2, 0.5);

	sceneManager->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(mPssmSetup));

	
	// Load and apply Skybox
	sceneManager->setSkyBox(true, "Examples/CloudyNoonSkyBox", 50000, true);

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
	//Entity* ent = sceneManager->createEntity("house.mesh");
	//SceneNode* entNode = sceneManager->createSceneNode("Character");
	//entNode->attachObject(ent);
	//sceneManager->getRootSceneNode()->addChild(entNode);
	//entNode->setPosition(100, 1000, -450);
	//entNode->scale(Vector3(20, 20, 20));
	//entNode->showBoundingBox(true);


	// Create an instance of the ImGui overlay and add it to the OverlayManager
	ImGuiOverlay *imguiOverlay = new ImGuiOverlay();
	imguiOverlay->setZOrder(300);
	imguiOverlay->show();
	Ogre::OverlayManager::getSingleton().addOverlay(imguiOverlay); // now owned by overlaymgr
	mImguiListener.reset(new ImGuiInputListener());
	addInputListener(mImguiListener.get());
	mListenerChain = InputListenerChain({ mImguiListener.get()});

	mTerrain = new CTerrain(sceneManager);
	mTerrain->Initialize(mPssmSetup);

	// Creating Model & Plant placing Instances
	mModelPlacer = new CModelPlacer(sceneManager, mTerrain);
	mPlantPlacer = new CPlantPlacer(sceneManager, mTerrain);
	mPlantPlacer->Initialize();
}

bool CGame::Update(const FrameEvent &evt)
{
	mModelPlacer->Update();
	mPlantPlacer->Update();

	mWindow->update();

	pollEvents();

	WindowEventUtilities::messagePump();

	ImGuiOverlay::NewFrame(evt);
	ImGui::ShowDemoWindow();

	// Debug Gui for FPS and Geometry stats
	RenderTarget::FrameStats stats = mWindow->getStatistics();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(400, mWindow->getHeight()));
	ImGui::Begin("", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::BeginChild("Stats", ImVec2(0, 90), true);
	ImGui::Text("FPS: %.2f (Avg: %.2f) -- MaxFPS: %.2f", stats.lastFPS, stats.avgFPS, stats.bestFPS);
	ImGui::Text("Triangle count: %d", stats.triangleCount);
	ImGui::Text("Batch count: %d", stats.batchCount);
	if (ImGui::Button("Exit!"))
	{
		exit(0);
	}
	ImGui::EndChild();
	return true;
}

void CGame::Render()
{
	// renderOneFrame() instead of beginRendering() to use own game loop
	mRoot->renderOneFrame();
}