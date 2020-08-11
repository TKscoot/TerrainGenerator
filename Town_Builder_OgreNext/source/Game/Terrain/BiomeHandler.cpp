#include "BiomeHandler.h"

CBiomeHandler::CBiomeHandler(Terrain * terrain)
						  : mTerrain(terrain)
{
	const uint16 terrainSize = mTerrain->getSize();
	
	for (int i = 0; i < 3; i++)
	{
		mCovImages[i].load("DefaultCovMap.png", ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

		StringStream ss;
		ss << "CovMap";
		ss << i;
		mCovTextures[i] = TextureManager::getSingletonPtr()->getByName(ss.str());
	}

	CalculateMoistureMap();

	for (int i = 0; i < mBiomeVegetationDescriptions.size(); i++)
	{
		mBiomeVegetationDescriptions[i].coverageMap.resize(terrainSize*terrainSize);

		// fill whole array with zeros
		std::fill(
			mBiomeVegetationDescriptions[i].coverageMap.begin(), 
			mBiomeVegetationDescriptions[i].coverageMap.end(), 0);

		mBiomeVegetationDescriptions[i].meshes.clear();
	}

	// Setting meshes to use for different biomes

	// Hill shrubs
	mBiomeVegetationDescriptions[3].meshes.push_back("plant1.mesh");
	mBiomeVegetationDescriptions[3].meshes.push_back("plant2.mesh");
	mBiomeVegetationDescriptions[3].meshes.push_back("farn1.mesh");
	mBiomeVegetationDescriptions[3].meshes.push_back("farn2.mesh");
	mBiomeVegetationDescriptions[3].meshes.push_back("shroom1_1.mesh");
	mBiomeVegetationDescriptions[3].meshes.push_back("shroom1_2.mesh");
	mBiomeVegetationDescriptions[3].meshes.push_back("shroom1_3.mesh");
	mBiomeVegetationDescriptions[3].meshes.push_back("shroom2_1.mesh");
	mBiomeVegetationDescriptions[3].meshes.push_back("shroom2_2.mesh");
	mBiomeVegetationDescriptions[3].meshes.push_back("shroom2_3.mesh");
	mBiomeVegetationDescriptions[3].poissonRadius = 80.0f;
	// Shrubland
	mBiomeVegetationDescriptions[4].meshes = mBiomeVegetationDescriptions[3].meshes;
	mBiomeVegetationDescriptions[4].poissonRadius = 40.0f;
	// Forest
	mBiomeVegetationDescriptions[5].meshes.push_back("fir05_30.mesh");
	mBiomeVegetationDescriptions[5].meshes.push_back("fir06_30.mesh");
	mBiomeVegetationDescriptions[5].meshes.push_back("fir14_25.mesh");
	//Jungle
	mBiomeVegetationDescriptions[7].meshes.push_back("fern_tree.mesh");
	mBiomeVegetationDescriptions[7].poissonRadius = 280.0f;
	mBiomeVegetationDescriptions[7].minSize = 0.75f;
	mBiomeVegetationDescriptions[7].maxSize = 2.25f;
	mBiomeVegetationDescriptions[9].meshes.push_back("fern_tree.mesh");
	mBiomeVegetationDescriptions[9].poissonRadius = 280.0f;
	mBiomeVegetationDescriptions[9].minSize = 0.75f;
	mBiomeVegetationDescriptions[9].maxSize = 2.25f;
	// Desert
	mBiomeVegetationDescriptions[8].meshes.push_back("cactus.mesh");
	mBiomeVegetationDescriptions[8].poissonRadius = 280.0f;
	mBiomeVegetationDescriptions[8].minSize = 0.75f;
	mBiomeVegetationDescriptions[8].maxSize = 3.25f;
	mBiomeVegetationDescriptions[10].meshes.push_back("cactus.mesh");
	mBiomeVegetationDescriptions[10].poissonRadius = 280.0f;
	mBiomeVegetationDescriptions[10].minSize = 0.75f;
	mBiomeVegetationDescriptions[10].maxSize = 3.25f;
	// Grassland
	mBiomeVegetationDescriptions[11].meshes.push_back("fir05_30.mesh");
	mBiomeVegetationDescriptions[11].meshes.push_back("fir06_30.mesh");
	mBiomeVegetationDescriptions[11].meshes.push_back("fir14_25.mesh");
	mBiomeVegetationDescriptions[11].poissonRadius = 120.0f;
											
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
}

Biomes CBiomeHandler::GetBiomeAtPoint(int x, int y)
{
	const uint16 terrainSize = mTerrain->getSize();
	float* map = mTerrain->getHeightData();

	float e = map[y * terrainSize + x];
	float m = mMoistureMap[y * terrainSize + x];

	// setting specific channel of coverage images to 1 for shader use
	if (e < 50)
	{
		mCovImages[0].setColourAt(ColourValue(1.0f, 0.0f, 0.0f, 0.0f), x, y, 0);
		return OCEAN;
	}
	if (e < 150)
	{
		mCovImages[0].setColourAt(ColourValue(0.0f, 1.0f, 0.0f, 0.0f), x, y, 0);
		return BEACH;
	}


	if (e > 2500)
	{
		mCovImages[0].setColourAt(ColourValue(0.0f, 0.0f, 1.0f, 0.0f), x, y, 0);
		return SNOW;
	}

	if (e > 800)
	{
		if (m < 0.15)
		{
			mCovImages[0].setColourAt(ColourValue(0.0f, 0.0f, 0.0f, 1.0f), x, y, 0);
			return TEMPERATE_DESERT;
		}
		if (m < 0.33)
		{
			mCovImages[1].setColourAt(ColourValue(1.0f, 0.0f, 0.0f, 0.0f), x, y, 0);
			return SHRUBLAND;
		}

		if (m < 0.66)
		{
			mCovImages[1].setColourAt(ColourValue(0.0f, 1.0f, 0.0f, 0.0f), x, y, 0);
			return TAIGA;
		}

		mCovImages[1].setColourAt(ColourValue(0.0f, 0.0f, 0.0f, 1.0f), x, y, 0);
		return TEMPERATE_DECIDUOUS_FOREST;
	}

	if (e > 400)
	{
		if (m < 0.16)
		{ 
			mCovImages[0].setColourAt(ColourValue(0.0f, 0.0f, 0.0f, 1.0f), x, y, 0);
			return TEMPERATE_DESERT;				 
		}
		if (m < 0.50)
		{ 
			mCovImages[1].setColourAt(ColourValue(0.0f, 0.0f, 1.0f, 0.0f), x, y, 0);
			return GRASSLAND;					 
		}
		if (m < 0.83)
		{ 
			mCovImages[1].setColourAt(ColourValue(0.0f, 0.0f, 0.0f, 1.0f), x, y, 0);
			return TEMPERATE_DECIDUOUS_FOREST;	 
		}

		mCovImages[2].setColourAt(ColourValue(1.0f, 0.0f, 0.0f, 0.0f), x, y, 0);
		return TEMPERATE_RAIN_FOREST;
	}

	if (m < 0.16)
	{
		mCovImages[2].setColourAt(ColourValue(0.0f, 1.0f, 0.0f, 0.0f), x, y, 0);
		return SUBTROPICAL_DESERT;
	}
	if (m < 0.33)
	{
		mCovImages[1].setColourAt(ColourValue(0.0f, 0.0f, 1.0f, 0.0f), x, y, 0);
		return GRASSLAND;
	}
	if (m < 0.66)
	{
		mCovImages[2].setColourAt(ColourValue(0.0f, 0.0f, 1.0f, 0.0f), x, y, 0);
		return TROPICAL_SEASONAL_FOREST;
	}

	mCovImages[2].setColourAt(ColourValue(0.0f, 0.0f, 0.0f, 1.0f), x, y, 0);
	return TROPICAL_RAIN_FOREST;
}

void CBiomeHandler::UpdateCoverageMap()
{
	const uint16 terrainSize = mTerrain->getSize();

	// reset all coverage maps
	for (int x = 0; x < terrainSize; x++)
	{
		for (int y = 0; y < terrainSize; y++)
		{
			for (int i = 0; i < 3; i++)
			{
				mCovImages[i].setColourAt(ColourValue(0.0f, 0.0f, 0.0f, 0.0f), x, y, 0);
			}
		}
	}

	mMoistureMap.clear();
	CalculateMoistureMap();

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

	// logging
	for (int i = 0; i < occurance.size(); i++)
	{
		std::cout << "Biome " << static_cast<Biomes>(i) << " occupies " << occurance[i] << " terrain points" << std::endl;
	}
	
	for (int i = 0; i < 3; i++)
	{
		mCovImages[i].flipAroundX();

		mCovTextures[i]->loadImage(mCovImages[i]);

		HardwarePixelBufferSharedPtr pixelBuffer = mCovTextures[i]->getBuffer();
		pixelBuffer->blitFromMemory(mCovImages[i].getPixelBox());
		pixelBuffer->unlock();
	}
}

void CBiomeHandler::CalculateMoistureMap()
{
	// using simplex noise to calculate moisture for all terrain points
	SimplexNoise::createPermutations(rand() % 10000);
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

			mMoistureMap.push_back(e);
		}
	}
}
