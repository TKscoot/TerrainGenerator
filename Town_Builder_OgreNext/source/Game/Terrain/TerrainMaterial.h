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

	TerrainMaterial(Ogre::String materialName, bool addNormalmap = true, bool cloneMaterial = true);

	void setMaterialByName(const Ogre::String materialName);
	void addNormalMapOnGenerate(bool set) { mAddNormalMap = set; };
	void cloneMaterialOnGenerate(bool set) { mCloneMaterial = set; };

	Ogre::String getMaterialName() { return mMaterialName; };

	class SM2ProfileA : public Ogre::TerrainMaterialGeneratorA::SM2Profile
	{
	public:
		SM2ProfileA(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc);
		virtual ~SM2ProfileA();

		//bool isVertexCompressionSupported() const { return false; }

		Ogre::MaterialPtr generate(const Ogre::Terrain* terrain) override;

		//Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain* terrain);
		//
		//Ogre::uint8 getMaxLayers(const Ogre::Terrain* terrain) const;
		//
		//void updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);
		//
		//void updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);
		//
		//void requestOptions(Ogre::Terrain* terrain);
		//
		//virtual void setLightmapEnabled(bool enabled);


	};

protected:
	Ogre::String mMaterialName;
	bool mCloneMaterial;
	bool mAddNormalMap;
};

