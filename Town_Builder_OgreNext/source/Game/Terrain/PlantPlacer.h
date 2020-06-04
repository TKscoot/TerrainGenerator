#pragma once
#include "Ogre.h"
#include "Terrain.h"
//#include "PagedGeometry/PagedGeometry.h"
//#include "PagedGeometry/BatchPage.h"
//#include "PagedGeometry/ImpostorPage.h"
//#include "PagedGeometry/TreeLoader3D.h"
//#include "PagedGeometry/GrassLoader.h"
#include "OgreWireBoundingBox.h"
#include "RTShaderSystem/OgreRTShaderSystem.h"

using namespace Ogre;
//using namespace Forests;


class CPlantPlacer
{
public:

	struct GrassVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};


	CPlantPlacer(SceneManager* sceneManager, CTerrain* terrain)
		: mSceneManager(sceneManager),
		  mTerrain(terrain)
	{
		/*
		mTreesPG = new PagedGeometry();
		mGrassPG = new PagedGeometry();
		*/
	}

	void Initialize();

	// NEEDS REFACTORING!!
	/*
	void PlaceTrees();
	void PlaceGrass();
	void CreateGrassMesh();
	*/
	void Update();

private:
	SceneManager* mSceneManager;
	CTerrain* mTerrain;
	
	/*
	PagedGeometry* mTreesPG;
	TreeLoader3D* mTreeLoader;
	PagedGeometry* mGrassPG;
	*/

	Entity* mEntity;
	SceneNode* mEntNode;


	const Real GRASS_WIDTH = 40;
	const Real GRASS_HEIGHT = 40;
	
};
