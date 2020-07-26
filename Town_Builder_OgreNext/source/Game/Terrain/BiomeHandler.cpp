#include "BiomeHandler.h"

CBiomeHandler::CBiomeHandler(Terrain * terrain)
						  : mTerrain(terrain)
{
	const uint16 terrainSize = mTerrain->getSize();

	img = new Image();
	img->load("DefaultCovMap.png", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

	// Splat manager init
	//mSplatMgr = new ET::SplattingManager("ETSplatting", "ET", terrainSize, terrainSize);
	//mSplatMgr->setNumTextures(6);

	float* map = mTerrain->getHeightData();

	for (int i = 0; i < (terrainSize * terrainSize); i++)
	{
		mNormalisedHeightMap.push_back(map[i]);
	}

	//mNormalisedHeightMap.assign(map, map + (terrainSize * terrainSize));

	//float minValue = std::numeric_limits<float>::max();
	//float maxValue = std::numeric_limits<float>::min();
	//
	//for (int i = 0; i < mNormalisedHeightMap.size(); i++)
	//{
	//	minValue = std::min(mNormalisedHeightMap[i], minValue);
	//	maxValue = std::max(mNormalisedHeightMap[i], maxValue);
	//}
	//
	//for (int i = 0; i < mNormalisedHeightMap.size(); i++)
	//{
	//	mNormalisedHeightMap[i] = (mNormalisedHeightMap[i] - minValue) / (maxValue - minValue);
	//}

	CalculateMoistureMap();

	// DEBUG!

	for (int i = 0; i < mBiomeVegetationDescriptions.size(); i++)
	{
		mBiomeVegetationDescriptions[i].coverageMap.resize(terrainSize*terrainSize);

		// fill whole array with zeros
		std::fill(
			mBiomeVegetationDescriptions[i].coverageMap.begin(), 
			mBiomeVegetationDescriptions[i].coverageMap.end(), 0);
	}


	std::array<int, (int)Biomes::BIOME_LAST> occurance = {};

	for (int i = 0; i < terrainSize; i++)
	{
		for (int j = 0; j < terrainSize; j++)
		{
			Biomes biome = GetBiomeAtPoint(i, j);
			occurance[static_cast<int>(biome)]++;
			mBiomeVegetationDescriptions[static_cast<int>(biome)].coverageMap[i * terrainSize + j] = 1.0f;
		}
	}

	for (int i = 0; i < occurance.size(); i++)
	{
		std::cout << "Biome " << static_cast<Biomes>(i) << " occupies " << occurance[i] << " terrain points" << std::endl;
	}

	img->save("media/materials/textures/CovMap1.png");

}

Biomes CBiomeHandler::GetBiomeAtPoint(int x, int y)
{
	const uint16 terrainSize = mTerrain->getSize();

	float e = mNormalisedHeightMap[y * terrainSize + x];
	float m = mMoistureMap[y * terrainSize + x];

	if (e < 50)
	{
		img->setColourAt(ColourValue(1.0f, 0.0f, 0.0f), x, y, 0);
		return OCEAN;
	}
	if (e < 70)
	{
		img->setColourAt(ColourValue(0.0f, 1.0f, 0.0f), x, y, 0);
		return BEACH;
	}

	if (e > 800)
	{
		img->setColourAt(ColourValue(0.0f, 0.0f, 1.0f), x, y, 0);
		return SNOW;
	}

	if (e > 600)
	{
		ColourValue cv = img->getColourAt(x, y, 0);
		img->setColourAt(ColourValue(cv.r, cv.g, cv.b, 0.0f), x, y, 0);

		if (m < 0.33) return TEMPERATE_DESERT;
		if (m < 0.66) return SHRUBLAND;
		return TAIGA;
	}

	if (e > 300)
	{
		if (m < 0.16) return TEMPERATE_DESERT;
		if (m < 0.50) return GRASSLAND;
		if (m < 0.83) return TEMPERATE_DECIDUOUS_FOREST;
		return TEMPERATE_RAIN_FOREST;
	}

	if (m < 0.16) return SUBTROPICAL_DESERT;
	if (m < 0.33) return GRASSLAND;
	if (m < 0.66) return TROPICAL_SEASONAL_FOREST;

	return TROPICAL_RAIN_FOREST;
}

void CBiomeHandler::UpdateCoverageMap()
{
	const uint16 terrainSize = mTerrain->getSize();
	float* map = mTerrain->getHeightData();

	for (int i = 0; i < (terrainSize * terrainSize); i++)
	{
		mNormalisedHeightMap.push_back(map[i]);
	}

	for (int i = 0; i < terrainSize; i++)
	{
		for (int j = 0; j < terrainSize; j++)
		{
			Biomes biome = GetBiomeAtPoint(i, j);
			mBiomeVegetationDescriptions[static_cast<int>(biome)].coverageMap[i * terrainSize + j] = 1.0f;
		}
	}
}

void CBiomeHandler::CalculateMoistureMap()
{
	const uint16 terrainSize = mTerrain->getSize();
	Vector2 worldOffset = mOriginPoint;
	Vector2 revisedValuePoint = Vector2::ZERO;

	for (uint16 i = 0; i < terrainSize; i++)
	{
		for (uint16 j = 0; j < terrainSize; j++)
		{
			revisedValuePoint = (worldOffset + Vector2(j, i)) / mCycle;

			float e = 0.0f;
			float heightScale = mHeightScale;
			float frequency = mFrequency;

			for (uint16 o = 0; o < mOctaves; o++)
			{
				e += SimplexNoise::noise(
					frequency * revisedValuePoint.x,
					frequency * revisedValuePoint.y)
					* heightScale;

				heightScale = heightScale * 0.5f;
				frequency = frequency * 2.0f;
			}

			//e = std::pow(e, mPowerFactor);

			mMoistureMap.push_back(e);
		}
	}
}
