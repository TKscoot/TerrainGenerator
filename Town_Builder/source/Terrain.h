#pragma once

#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0

//#include "Game.h"
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainGroup.h"
#include "SimplexNoise.h"
#include "Helpers.h"
#include "Ogre.h"
#include "OIS.h"
#include <ctime>

#define TERRAIN_FILE_PREFIX String("testTerrain")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 2049

using namespace Ogre;

class CTerrain
{
public:
	CTerrain(SceneManager* scnMgr) : mSceneManager(scnMgr) {}
	void Initialize(AxisAlignedBox box);
	void Update();
	
	void FlattenTerrainUnderObject(SceneNode* sn);

	void InjectKeyDown(const OIS::KeyEvent& evt);
	void InjectKeyUp(const OIS::KeyEvent& evt);

	// Getter & Setter
	TerrainGroup* getTerrainGroup() { return mTerrainGroup; }

private:
	// Methods
	void ConfigureTerrainDefaults(Light* l);

	void DefineTerrain(long x, long y, float value = 0.0f);
	void GetTerrainImage(bool flipX, bool flipY, Image& img);
	void InitBlendMaps(Terrain* terrain);

	// structs
	struct HeightmapCorrectionData
	{
		Vector3 corner = Vector3();
		Vector3 terrainPosition;

	};

	// Vars
	SceneManager* mSceneManager;

	Ogre::TerrainGlobalOptions* mTerrainGlobals;
	Ogre::TerrainGroup* mTerrainGroup;
	Terrain* mTerrain;


	Vector3 mTerrainPos;
	bool mTerrainsImported = false;
	Real mCycle = 1024;
	Real mHeightScale = 4.0f;
	Vector2 mOriginPoint;

	float mFrequency = 1;
	int mOctaves = 8;

	float mSeed = 0;

	AxisAlignedBox ogreBox;

	bool mKeyPressed = false;

};