#pragma once
#include <iostream>
#include <algorithm>

#include <Ogre.h>
#include <Terrain/OgreTerrain.h>

#include "Common/SimplexNoise.h"
#include "ETM/ETSplattingManager.h"

#include "Game/PoissonMeshInstance.h"

using namespace Ogre;
template <typename EnumType, typename... More>
void write_enum_value(std::ostream& out, EnumType t,
	const char* names, size_t index, EnumType head, More... tail)
{
	if (t == head)
	{
		for (size_t i = 0; i < index; ++i)
		{
			names = strchr(names, ',');
			names += strspn(names, " ,");
		}
		out.write(names, strcspn(names, " ,"));
	}
	else
	{
		write_enum_value(out, t, names, index + 1, tail...);
	}
}

template <typename EnumType>
void write_enum_value(std::ostream& out, EnumType t,
	const char*, size_t)
{
	out << long(t);
}

#define DEFINE_ENUM(EnumType, first_value, first_name, ...) \
    enum EnumType { first_name = first_value, __VA_ARGS__ }; \
    inline std::ostream& operator<<(std::ostream& out, EnumType t) { \
        write_enum_value(out, t, # first_name "," # __VA_ARGS__, 0, \
            first_name, __VA_ARGS__); \
        return out; \
    }

DEFINE_ENUM(Biomes,
	0,
	OCEAN,
	BEACH,
	SNOW,
	TEMPERATE_DESERT,
	SHRUBLAND,
	TAIGA,
	GRASSLAND,
	TEMPERATE_DECIDUOUS_FOREST,
	TEMPERATE_RAIN_FOREST,
	SUBTROPICAL_DESERT,
	TROPICAL_SEASONAL_FOREST,
	TROPICAL_RAIN_FOREST,
	BIOME_LAST
)

//DEFINE_ENUM(Biomes,
//	0,
//	OCEAN,
//	BEACH,
//	TUNDRA,
//	TEMPERATE_GRASSLAND,
//	BOREAL_FOREST,
//	WOODLAND,
//	TEMPERATE_SEASONAL_FOREST,
//	TEMPERATE_RAIN_FOREST,
//	SUBTROPICAL_DESERT,
//	SAVANNA,
//	TROPICAL_RAINFOREST,
//	BIOME_LAST
//)

class CBiomeHandler
{
public:
	CBiomeHandler(Terrain* terrain);

	Biomes GetBiomeAtPoint(int x, int y);

	void UpdateCoverageMap();

	struct BiomeVegetationDescription
	{
		std::vector<float>	  coverageMap;
		std::vector<String>   meshes;
		CPoissonMeshInstance* meshInstance;
	};

	std::array<BiomeVegetationDescription, static_cast<int>(Biomes::BIOME_LAST)> &GetBiomeVegetationDescriptions() 
	{ 
		return mBiomeVegetationDescriptions; 
	}

private:

	// Methods
	void CalculateMoistureMap();

	// Variables
	Terrain* mTerrain = nullptr;
	//ET::SplattingManager* mSplatMgr = nullptr;


	std::vector<float> mNormalisedHeightMap;
	std::vector<float> mMoistureMap;

	std::array<BiomeVegetationDescription, static_cast<int>(Biomes::BIOME_LAST)> mBiomeVegetationDescriptions;

	// moisture map params
	Real	mCycle = 1024;
	Real	mHeightScale = 2.0f;
	Vector2 mOriginPoint = { 0, 0 };

	float	mFrequency = 1.0f;
	float   mPowerFactor = 2.0f;
	int		mOctaves = 5;

	Image* img;

};