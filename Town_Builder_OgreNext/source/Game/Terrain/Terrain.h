#pragma once

#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0

//#include "Game.h"
#include <ctime>
#include <iostream>

#include "Ogre.h"
#include "OgreImGuiOverlay.h"
#include "imgui.h"
#include "Terrain/OgreTerrainMaterialGeneratorA.h"
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainGroup.h"
#include "TerrainMaterial.h"

#include "Common/SimplexNoise.h"
#include "Common/Helpers.h"
#include "Engine/InputManager.h"
#include "Engine/Event.h"
#include "HydraulicErosion.h"
#include "PlantPlacer.h"
#include "BiomeHandler.h"

#define TERRAIN_FILE_PREFIX String("testTerrain")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f // how bi the terrain is in WorldSpace
#define TERRAIN_SIZE 513 // terrain size must be x^2+1

using namespace Ogre;

class CTerrain
{
public:
	CTerrain(SceneManager* scnMgr) : mSceneManager(scnMgr) {}
	~CTerrain();

	void Initialize(PSSMShadowCameraSetup* pssmSetup);
	bool Update(const FrameEvent& evt);

	void UpdateSeed();
	void CreateTerrain();

	void Erode(int iterations);

	// Getter & Setter
	TerrainGroup* getTerrainGroup() 
	{ 
		return mTerrainGroup; 
	}

	TexturePtr getColorMap() 
	{ 
		return mTerrain->getGlobalColourMap(); 
	}

private:
	// Methods

	void ConfigureTerrainDefaults();

	// Defines initial Terrain
	void DefineTerrain(long x, long y);
	// Updates heightdata of already generated terrain
	void UpdateTerrainHeight(long x, long y);
	void InitBlendMaps(Terrain* terrain);

	// structs
	struct HeightmapCorrectionData
	{
		Vector3 corner = Vector3();
		Vector3 terrainPosition;
	};

	// Vars
	SceneManager*		   mSceneManager;
	PSSMShadowCameraSetup* mPssmSetup = nullptr;


	TerrainGlobalOptions* mTerrainGlobals;
	TerrainGroup*		  mTerrainGroup;
	Terrain*			  mTerrain;
	TerrainMaterial*	  mTerrainMaterial;
	CPlantPlacer*         mPlantPlacer;
	CBiomeHandler*        mBiomeHandler;

	Vector3 mTerrainPos		  = Vector3::ZERO;
	bool	mTerrainsImported = false;
	Real	mCycle			  = 1024;
	Real	mHeightScale	  = 2.0f;
	Vector2 mOriginPoint	  = { 0, 0 };

	float	mFrequency   = 1.0f;
	float   mPowerFactor = 2.0f;
	int		mOctaves     = 5;

	bool mPlaceVegetation = false;

	// erosion
	CHydraulicErosion* mErosion = nullptr;

	int	mErosionIterations = 10;

	std::string	mSeed = "";

};