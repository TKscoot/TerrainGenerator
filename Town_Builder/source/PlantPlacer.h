#pragma once
#include "Ogre.h"
#include "Terrain.h"

using namespace Ogre;

class CPlantPlacer
{
public:
	CPlantPlacer(SceneManager* sceneManager, CTerrain* terrain)
		: mSceneManager(sceneManager),
		  mTerrain(terrain){}

private:
	SceneManager* mSceneManager;
	CTerrain* mTerrain;
	
};
