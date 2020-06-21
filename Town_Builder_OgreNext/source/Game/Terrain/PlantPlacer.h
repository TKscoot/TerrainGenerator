#pragma once
#include "Ogre.h"
#include "Terrain.h"
#include "PagedGeometryNew/PagedGeometry.h"
#include "PagedGeometryNew/BatchPage.h"
#include "PagedGeometryNew/ImpostorPage.h"
#include "PagedGeometryNew/TreeLoader3D.h"
#include "PagedGeometryNew/GrassLoader.h"
#include "OgreWireBoundingBox.h"
#include "RTShaderSystem/OgreRTShaderSystem.h"

#include "Common/Poisson.h"
#include "Engine/Event.h"
#include "Engine/InputManager.h"
#include "Game/PoissonMeshInstance.h"

using namespace Ogre;
using namespace Forests;

#define NUM_TECHNIQUES (((int)InstanceManager::InstancingTechniquesCount) + 1)

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

	}
	void Initialize();
	bool Update(const FrameEvent &evt);
	void Finalize();

private:

	// Methods
	void PlaceTrees();
	void ClearTrees();
	void CreateGrassMesh();


	// Vars
	SceneManager*		  mSceneManager		 = nullptr;
	CTerrain*			  mTerrain			 = nullptr;
	CPoissonMeshInstance* mInstancingManager = nullptr;


	PagedGeometry* mTreePG = nullptr;

	//Parameters
	float mPoissonRadius = 100.0f;
	float mMinHeight = 50.0f;
	float mMaxHeight = 450.0f;

	std::vector<SceneNode*> mPlacedTrees;

	// Grass
	const Real GRASS_WIDTH = 40;
	const Real GRASS_HEIGHT = 40;

};
