#pragma once
#include "Ogre.h"
#include "OIS.h"
#include "Terrain.h"

using namespace Ogre;

class CModelPlacer
{
public:
	CModelPlacer(SceneManager* sceneManager, CTerrain* terrain) 
		: mSceneManager(sceneManager),
		  mTerrain(terrain) 
	{
		id++;
		std::stringstream ss;
		ss << id;
		std::string s = ss.str();



		mEntity = mSceneManager->createEntity("Sinbad.mesh");
		mEntNode = mSceneManager->getRootSceneNode()->createChildSceneNode(s);
		mEntNode->attachObject(mEntity);
		mEntNode->_update(true, false);
		mEntNode->scale(Vector3(10, 10, 10));
	}

	void Update();
	void PlaceObject();
	void TintObject(float r, float g, float b, float a);

	float CalculateHeightDelta(Vector3 position);

	void InjectMousePress(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	void InjectKeyDown(const OIS::KeyEvent& evt);


private:

	SceneManager* mSceneManager = nullptr;
	CTerrain* mTerrain = nullptr;
	int id = 0;

	Entity* mEntity;
	SceneNode* mEntNode;
	bool mAttachToMouse = false;
	MaterialPtr mMatPtr;


};