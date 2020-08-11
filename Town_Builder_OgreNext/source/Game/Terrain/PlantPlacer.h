#pragma once
#include "Ogre.h"
#include "OgreWireBoundingBox.h"
#include "RTShaderSystem/OgreRTShaderSystem.h"

#include "Common/Poisson.h"
#include "Engine/Event.h"
#include "Engine/InputManager.h"
#include "Game/PoissonMeshInstance.h"
#include "Game/Terrain/BiomeHandler.h"
#include "Common/SimplexNoise.h"

using namespace Ogre;

class CPlantPlacer
{
public:

	// Vertex Data for custom grass mesh
	struct GrassVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

	CPlantPlacer(SceneManager* sceneManager, Terrain* terrain, CBiomeHandler* biomeHandler)
		: mSceneManager(sceneManager),
		  mTerrain(terrain),
		  mBiomeHandler(biomeHandler)
	{
	}

	void Initialize();

	void Finalize();
	bool Update(const FrameEvent &evt);

	void PlaceVegetation();
	void ClearVegetation();

private:

	// Methods

	void CreateGrassMesh();


	// Vars
	SceneManager*		  mSceneManager	  = nullptr;
	Terrain*			  mTerrain		  = nullptr;
	CBiomeHandler*		  mBiomeHandler   = nullptr;

	std::array<CBiomeHandler::BiomeVegetationDescription, static_cast<int>(BIOME_LAST)> mBiomeVegDescs;

	// Grass
	const Real GRASS_WIDTH = 40;
	const Real GRASS_HEIGHT = 40;

	std::vector<const char*> mBiomeNames;
};
