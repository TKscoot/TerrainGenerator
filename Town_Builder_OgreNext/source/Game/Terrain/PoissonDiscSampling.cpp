#include "PoissonDiscSampling.h"

std::vector<Vector2> CPoissonDiscSampling::GeneratePoints(float radius, Vector2 sampleRegionSize, int numSamplesBeforeRejection)
{
	srand(time(NULL));

	float cellSize = radius / std::sqrt(2);

	int gridSizeX = sampleRegionSize.x / cellSize;
	int gridSizeY = sampleRegionSize.y / cellSize;

	std::vector<std::vector<int>> grid(
		gridSizeX,
		std::vector<int>(gridSizeY));

	std::vector<Vector2> points;
	std::vector<Vector2> spawnPoints;

	spawnPoints.push_back(Vector2(sampleRegionSize / 2));

	while (spawnPoints.size() > 0)
	{
		int spawnIndex = rand() % spawnPoints.size(); // +1 eventuell buggy!!
		Vector2 spawnCentre = spawnPoints[spawnIndex];

		bool candidateAccepted = false;

		for (int i = 0; i < numSamplesBeforeRejection; i++)
		{
			float angle = Helpers::RandomFloat(0.0f, 1.0f) * Math::PI * 2;
			Vector2 dir = Vector2(Math::Sin(angle), Math::Cos(angle));
			Vector2 candidate = spawnCentre + dir * Helpers::RandomFloat(radius, 2 * radius);

			if (IsValid(candidate, sampleRegionSize, cellSize, radius, points, grid))
			{
				points.push_back(candidate);
				spawnPoints.push_back(candidate);
				grid[(candidate.x / cellSize)][ (candidate.y / cellSize)] = static_cast<int>(points.size());

				candidateAccepted = true;
				break;
			}

		}

		if (!candidateAccepted)
		{
			spawnPoints.erase(spawnPoints.begin() + spawnIndex);	// eventuell falscher index (wegen 0 und so)
		}

	}


	return points;
}

bool CPoissonDiscSampling::IsValid(Vector2 candidate, Vector2 sampleRegionSize, float cellSize, float radius, std::vector<Vector2> points, std::vector<std::vector<int>> grid)
{
	if (candidate.x >= 0 && candidate.x < sampleRegionSize.x && candidate.y >= 0 && candidate.y < sampleRegionSize.y)
	{
		int cellX = (int)(candidate.x / cellSize);
		int cellY = (int)(candidate.y / cellSize);
		int searchStartX = std::max(0, cellX - 2);
		int searchEndX = std::min(cellX + 2, static_cast<int>(grid.size()) - 1);
		int searchStartY = std::max(0, cellY - 2);
		int searchEndY = std::min(cellY + 2, static_cast<int>(grid[0].size()) - 1);

		for (int x = searchStartX; x <= searchEndX; x++)
		{
			for (int y = searchStartY; y <= searchEndY; y++)
			{
				int pointIndex = grid[x][y] - 1;
				if (pointIndex != -1)
				{
					Vector2 sqrVec = candidate - points[pointIndex];
					//float sqrDst = (candidate - points[pointIndex]).sqrMagnitude;
					//float sqrDst = sqrVec.squaredLength(); //eventuel buggy
					float sqrDst = sqrVec.squaredLength(); //eventuel buggy
					if (sqrDst < radius*radius)
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	return false;
}
