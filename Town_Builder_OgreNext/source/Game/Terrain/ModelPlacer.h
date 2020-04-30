#pragma once
#include "Ogre.h"
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

		mEntity = mSceneManager->createEntity("house.mesh");
		mEntNode = mSceneManager->getRootSceneNode()->createChildSceneNode(ss.str());
		mEntNode->attachObject(mEntity);
		mEntNode->_update(true, false);
		mEntNode->scale(Vector3(10, 10, 10));
	}

	void Update();
	void PlaceObject();
	void TintObject(Entity* ent, float r, float g, float b, float a);
	void CloneAndApplyMaterial(Entity* ent);

	float CalculateHeightDelta(Vector3 position);

	void InjectMousePress(const MouseButtonEvent& evt);
	void InjectKeyDown(const KeyboardEvent& evt);


private:
	enum TintState{FULL, GREEN, RED};

	SceneManager* mSceneManager = nullptr;
	CTerrain* mTerrain = nullptr;
	int id = 0;

	Entity* mEntity;
	SceneNode* mEntNode;
	bool mAttachToMouse = false;
	TintState mTintState = FULL;
	MaterialPtr mMatPtr;

	std::vector<SceneNode*> mPlacedObjects;
};