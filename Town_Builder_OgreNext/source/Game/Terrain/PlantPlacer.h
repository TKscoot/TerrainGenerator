#pragma once
#include "Ogre.h"
#include "PagedGeometry/PagedGeometry.h"
#include "PagedGeometry/BatchPage.h"
#include "PagedGeometry/ImpostorPage.h"
#include "PagedGeometry/TreeLoader3D.h"
#include "PagedGeometry/GrassLoader.h"
#include "OgreWireBoundingBox.h"
#include "RTShaderSystem/OgreRTShaderSystem.h"

#include "Common/Poisson.h"
#include "Engine/Event.h"
#include "Engine/InputManager.h"
#include "Game/PoissonMeshInstance.h"
#include "Common/SimplexNoise.h"

using namespace Ogre;
using namespace Forests;

class CPlantPlacer
{
public:

	struct GrassVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};


	CPlantPlacer(SceneManager* sceneManager, Terrain* terrain)
		: mSceneManager(sceneManager),
		  mTerrain(terrain)
	{
	}

	void Initialize();
	bool Update(const FrameEvent &evt)
	{
		return true;
	}

	void Finalize();

	void PlaceVegetation();

private:

	// Methods

	void CreateGrassMesh();


	// Vars
	SceneManager*		  mSceneManager	  = nullptr;
	Terrain*			  mTerrain		  = nullptr;
	CPoissonMeshInstance* mInstancedGrass = nullptr;
	CPoissonMeshInstance* mInstancedRocks = nullptr;
	CPoissonMeshInstance* mTree			  = nullptr;
	CPoissonMeshInstance* mFoliage		  = nullptr;


	// Grass
	const Real GRASS_WIDTH = 40;
	const Real GRASS_HEIGHT = 40;
};
