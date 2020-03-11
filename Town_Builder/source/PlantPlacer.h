#pragma once
#include "Ogre.h"
#include "Terrain.h"
#include "PagedGeometry/PagedGeometry.h"
#include "PagedGeometry/BatchPage.h"
#include "PagedGeometry/ImpostorPage.h"
#include "PagedGeometry/TreeLoader3D.h"
#include "PagedGeometry/GrassLoader.h"
#include "OgreWireBoundingBox.h"

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
		mGrassPG = new PagedGeometry();
	}

	void Initialize();
	void PlaceTrees();
	void PlaceGrass();
	void Update();

private:
	SceneManager* mSceneManager;
	CTerrain* mTerrain;
	
	PagedGeometry* mTreesPG;
	TreeLoader3D* mTreeLoader;
	Entity* mEntity;
	SceneNode* mEntNode;

	PagedGeometry* mGrassPG;
	
};
