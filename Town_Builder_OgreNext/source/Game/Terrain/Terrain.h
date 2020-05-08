#pragma once

#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0

//#include "Game.h"
#include <ctime>
#include <iostream>
#include "Ogre.h"
#include "Terrain/OgreTerrainMaterialGeneratorA.h"
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainGroup.h"
#include "OgreImGuiOverlay.h"
#include "Common/SimplexNoise.h"
#include "Common/Helpers.h"
#include "Engine/InputManager.h"
#include "imgui.h"
#include "Engine/Event.h"
#include "AI/Recast/Helpers/RecastInputGeom.h"
#include "Erosion.h"

#define TERRAIN_FILE_PREFIX String("testTerrain")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 2049

using namespace Ogre;

class CTerrain
{
public:
	CTerrain(SceneManager* scnMgr) : mSceneManager(scnMgr) {}
	void Initialize(PSSMShadowCameraSetup* pssmSetup);
	bool Update(const FrameEvent& evt);
	void CreateTerrain();
	
	void FlattenTerrainUnderObject(SceneNode* sn);

	// Getter & Setter
	TerrainGroup* getTerrainGroup() 
	{ 
		return mTerrainGroup; 
	}

	TexturePtr getColorMap() 
	{ 
		return mTerrain->getGlobalColourMap(); 
	}

	bool IsUpdated() { return mIsUpdated; }
	InputGeom* GetInputGeom() { return mInputGeom; }

private:
	// Methods
	void ConfigureTerrainDefaults();

	void DefineTerrain(long x, long y);
	void UpdateTerrainHeight(long x, long y);
	void GetTerrainImage(bool flipX, bool flipY, Image& img);
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
	InputGeom*			  mInputGeom;
	CErosion*             mErosion;

	Vector3 mTerrainPos;
	bool	mTerrainsImported = false;
	Real	mCycle			  = 1024;
	Real	mHeightScale	  = 1.0f;
	Vector2 mOriginPoint	  = { 0, 0 };

	float	mFrequency = 0.5f;
	float   mPowerFactor = 2.0f;
	int		mOctaves = 8;

	std::string	mSeed = "";
	bool mIsUpdated = false;
};