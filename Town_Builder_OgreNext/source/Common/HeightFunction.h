#pragma once
#include "Terrain/OgreTerrainGroup.h"

namespace HeightFunction
{
	Ogre::Terrain* terrain = nullptr;

	//Initializes the height function. Call this before calling getTerrainHeight()
	void initialize(Ogre::Terrain* terrainGroup)
	{
		if (!terrain)
		{
			terrain = terrainGroup;
		}
	}

	//Gets the height of the terrain at the specified x/z coordinate
	//The userData parameter isn't used in this implementation of a height function, since
	//there's no need for extra data other than the x/z coordinates.
	inline float getTerrainHeight(const float x, const float z, void *userData = nullptr)
	{
		return terrain->getHeightAtWorldPosition(x, 0, z);
	}
}