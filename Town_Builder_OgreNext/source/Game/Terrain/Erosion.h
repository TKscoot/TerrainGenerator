#pragma once
#include <vector>
#include <ctime>
#include <math.h>
#include <algorithm>

class CErosion
{
public:
	struct HeightAndGradient
	{
		float height;
		float gradientX;
		float gradientY;
	};






	// Initialization creates a System.Random object and precomputes indices and weights of erosion brush
	void Initialize(int mapSize, bool resetSeed)
	{
		if (resetSeed || currentSeed != mSeed)
		{
			srand(time(NULL));
			mSeed = rand() % 50000;
			currentSeed = mSeed;
		}

		if (mErosionBrushIndices.empty() || currentErosionRadius != mErosionRadius || currentMapSize != mapSize)
		{
			InitializeBrushIndices(mapSize, mErosionRadius);
			currentErosionRadius = mErosionRadius;
			currentMapSize = mapSize;
		}
	}

	void Erode(float map[], int mapSize, int numIterations = 1, bool resetSeed = false)
	{
		Initialize(mapSize, resetSeed);

		for (int iteration = 0; iteration < numIterations; iteration++)
		{
			// Create water droplet at random point on map
			float posX = rand() % (mapSize - 1);
			float posY = rand() % (mapSize - 1);
			float dirX = 0;
			float dirY = 0;
			float speed = mInitialSpeed;
			float water = mInitialWaterVolume;
			float sediment = 0;

			for (int lifetime = 0; lifetime < mMaxDropletLifetime; lifetime++)
			{
				int nodeX = (int)posX;
				int nodeY = (int)posY;
				int dropletIndex = nodeY * mapSize + nodeX;
				// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
				float cellOffsetX = posX - nodeX;
				float cellOffsetY = posY - nodeY;

				// Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
				HeightAndGradient heightAndGradient = CalculateHeightAndGradient(map, mapSize, posX, posY);

				// Update the droplet's direction and position (move position 1 unit regardless of speed)
				dirX = (dirX * mInertia - heightAndGradient.gradientX * (1 - mInertia));
				dirY = (dirY * mInertia - heightAndGradient.gradientY * (1 - mInertia));
				// Normalize direction
				//float len = Mathf.Sqrt(dirX * dirX + dirY * dirY);
				float len = sqrt(dirX * dirX + dirY * dirY);
				if (len != 0)
				{
					dirX /= len;
					dirY /= len;
				}

				posX += dirX;
				posY += dirY;

				// Stop simulating droplet if it's not moving or has flowed over edge of map
				if ((dirX == 0 && dirY == 0) || posX < 0 || posX >= mapSize - 1 || posY < 0 || posY >= mapSize - 1)
				{
					break;
				}

				// Find the droplet's new height and calculate the deltaHeight
				float newHeight = CalculateHeightAndGradient(map, mapSize, posX, posY).height;
				float deltaHeight = newHeight - heightAndGradient.height;

				// Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
				//float sedimentCapacity = Mathf.Max(-deltaHeight * speed * water * mSedimentCapacityFactor, mMinSedimentCapacity);
				float sedimentCapacity = std::max(-deltaHeight * speed * water * mSedimentCapacityFactor, mMinSedimentCapacity);

				// If carrying more sediment than capacity, or if flowing uphill:
				if (sediment > sedimentCapacity || deltaHeight > 0)
				{
					// If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
					float amountToDeposit = (deltaHeight > 0) ? std::min(deltaHeight, sediment) : (sediment - sedimentCapacity) * mDepositSpeed;
					sediment -= amountToDeposit;

					// Add the sediment to the four nodes of the current cell using bilinear interpolation
					// Deposition is not distributed over a radius (like erosion) so that it can fill small pits
					map[dropletIndex] += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetY);
					map[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1 - cellOffsetY);
					map[dropletIndex + mapSize] += amountToDeposit * (1 - cellOffsetX) * cellOffsetY;
					map[dropletIndex + mapSize + 1] += amountToDeposit * cellOffsetX * cellOffsetY;

				}
				else
				{
					// Erode a fraction of the droplet's current carry capacity.
					// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
					float amountToErode = std::min((sedimentCapacity - sediment) * mErodeSpeed, -deltaHeight);

					// Use erosion brush to erode from all nodes inside the droplet's erosion radius
					for (int brushPointIndex = 0; brushPointIndex < mErosionBrushIndices[dropletIndex].size(); brushPointIndex++)
					{
						int nodeIndex = mErosionBrushIndices[dropletIndex][brushPointIndex];
						float weighedErodeAmount = amountToErode * mErosionBrushIndices[dropletIndex][brushPointIndex];
						float deltaSediment = (map[nodeIndex] < weighedErodeAmount) ? map[nodeIndex] : weighedErodeAmount;
						map[nodeIndex] -= deltaSediment;
						sediment += deltaSediment;
					}
				}

				// Update droplet's speed and water content
				speed = sqrt(speed * speed + deltaHeight * mGravity);
				water *= (1 - mEvaporateSpeed);
			}
		}
	}

	HeightAndGradient CalculateHeightAndGradient(float nodes[], int mapSize, float posX, float posY)
	{
		int coordX = (int)posX;
		int coordY = (int)posY;

		// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
		float x = posX - coordX;
		float y = posY - coordY;

		// Calculate heights of the four nodes of the droplet's cell
		int nodeIndexNW = coordY * mapSize + coordX;
		float heightNW = nodes[nodeIndexNW];
		float heightNE = nodes[nodeIndexNW + 1];
		float heightSW = nodes[nodeIndexNW + mapSize];
		float heightSE = nodes[nodeIndexNW + mapSize + 1];

		// Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
		float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
		float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

		// Calculate height with bilinear interpolation of the heights of the nodes of the cell
		float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;

		HeightAndGradient hag { height, gradientX, gradientY };

		return hag;
	}

	void InitializeBrushIndices(int mapSize, int radius)
	{
		mErosionBrushIndices.resize(mapSize * mapSize);
		mErosionBrushWeights.resize(mapSize * mapSize);

		std::vector<int>   xOffsets; //= new int[radius * radius * 4];
		xOffsets.resize(radius * radius * 4);
		std::vector<int>   yOffsets; //= new int[radius * radius * 4];
		yOffsets.resize(radius * radius * 4);

		std::vector<float> weights;  //= new float[radius * radius * 4];
		weights.resize(radius * radius * 4);


		float weightSum = 0;
		int addIndex = 0;

		for (int i = 0; i < mErosionBrushIndices.size(); i++)
		{
			int centreX = i % mapSize;
			int centreY = i / mapSize;

			if (centreY <= radius || centreY >= mapSize - radius || centreX <= radius + 1 || centreX >= mapSize - radius)
			{
				weightSum = 0;
				addIndex = 0;
				for (int y = -radius; y <= radius; y++)
				{
					for (int x = -radius; x <= radius; x++)
					{
						float sqrDst = x * x + y * y;
						if (sqrDst < radius * radius)
						{
							int coordX = centreX + x;
							int coordY = centreY + y;

							if (coordX >= 0 && coordX < mapSize && coordY >= 0 && coordY < mapSize)
							{
								float weight = 1 - sqrt(sqrDst) / radius;
								weightSum += weight;
								weights[addIndex] = weight;
								xOffsets[addIndex] = x;
								yOffsets[addIndex] = y;
								addIndex++;
							}
						}
					}
				}
			}

			int numEntries = addIndex;
			mErosionBrushIndices[i].resize(numEntries);
			mErosionBrushIndices[i].resize(numEntries);
			mErosionBrushWeights[i].resize(numEntries);

			for (int j = 0; j < numEntries; j++)
			{
				mErosionBrushIndices[i][j] = (yOffsets[j] + centreY) * mapSize + xOffsets[j] + centreX;
				mErosionBrushWeights[i][j] = weights[j] / weightSum;
			}
		}
	}









private:
	// Parameters
	int   mSeed					  = 0;
	int   mErosionRadius		  = 3;
	float mInertia				  = 0.05f;
	float mSedimentCapacityFactor = 4.0f;
	float mMinSedimentCapacity	  = 0.01f;
	float mErodeSpeed			  = 0.3f;
	float mDepositSpeed			  = 0.3f;
	float mEvaporateSpeed		  = 0.01f;
	float mGravity				  = 4.0f;
	int   mMaxDropletLifetime	  = 30;
	float mInitialWaterVolume     = 1.0f;
	float mInitialSpeed			  = 1.0f;

	std::vector<std::vector<int>>   mErosionBrushIndices;
	std::vector<std::vector<float>> mErosionBrushWeights;


	int currentSeed;
	int currentErosionRadius;
	int currentMapSize;

};