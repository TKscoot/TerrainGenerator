#pragma once
#include <vector>
#include <iostream>
#include <Ogre.h>

#include "Common/Helpers.h"

using namespace Ogre;

class CPoissonDiscSampling
{
public:

	std::vector<Vector2> GeneratePoints(float radius, Vector2 sampleRegionSize, int numSamplesBeforeRejection = 30);

private:

	bool IsValid(Vector2 candidate, Vector2 sampleRegionSize, float cellSize, float radius, std::vector<Vector2> points, std::vector<std::vector<int>> grid);

};