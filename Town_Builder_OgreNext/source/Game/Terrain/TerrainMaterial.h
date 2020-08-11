// Simple Material Generator by Nauk
#pragma once
#include <iostream>
#include "Ogre.h"
#include "Terrain/OgreTerrain.h"
#include "Terrain/OgreTerrainMaterialGenerator.h"
#include "Terrain/OgreTerrainMaterialGeneratorA.h"

using namespace Ogre;

class TerrainMaterial : public Ogre::TerrainMaterialGeneratorA
{
public:

	TerrainMaterial(Ogre::String materialName);

	void setMaterialByName(const Ogre::String materialName);
	Ogre::String getMaterialName() { return mMaterialName; };

	// Use SM2Profile as Template for own Terrain Material Profile
	class SM2ProfileA : public Ogre::TerrainMaterialGeneratorA::SM2Profile
	{
	public:
		SM2ProfileA(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc);
		virtual ~SM2ProfileA();

		// overridden methods
		Ogre::MaterialPtr generate(const Ogre::Terrain* terrain) override;
		void requestOptions(Ogre::Terrain* terrain);


	};

protected:
	Ogre::String mMaterialName;
};

