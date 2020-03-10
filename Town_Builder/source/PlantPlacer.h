#pragma once
#include "Ogre.h"
#include "Terrain.h"
#include "PagedGeometry/PagedGeometry.h"
#include "PagedGeometry/BatchPage.h"
#include "PagedGeometry/ImpostorPage.h"
#include "PagedGeometry/TreeLoader3D.h"

using namespace Ogre;
using namespace Forests;


class CPlantPlacer
{
public:
	CPlantPlacer(SceneManager* sceneManager, CTerrain* terrain)
		: mSceneManager(sceneManager),
		  mTerrain(terrain)
	{
		mTreesPG = new PagedGeometry();
	}

	void Initialize();
	void Update();

private:
	SceneManager* mSceneManager;
	CTerrain* mTerrain;
	
	PagedGeometry* mTreesPG;
	TreeLoader3D* mTreeLoader;
	Entity* mEntity;
	SceneNode* mEntNode;
	
};
