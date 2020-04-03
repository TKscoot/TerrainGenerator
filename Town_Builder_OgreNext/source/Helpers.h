#pragma once
#include <cstdlib>
#include <ctime>

namespace Helpers
{
	static float RandomFloat(float a, float b)
	{
		std::srand(std::time(nullptr));
		float random = ((float)std::rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}
}
