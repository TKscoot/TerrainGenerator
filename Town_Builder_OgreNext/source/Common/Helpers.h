#pragma once
#include <cstdlib>
#include <ctime>
#include "imgui.h"

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

namespace OgreRecast
{
	static void OgreVect3ToFloatA(const Ogre::Vector3 vect, float* result)
	{
		result[0] = vect[0];
		result[1] = vect[1];
		result[2] = vect[2];
	};

	static void FloatAToOgreVect3(const float* vect, Ogre::Vector3 &result)
	{
		result.x = vect[0];
		result.y = vect[1];
		result.z = vect[2];
	}
}
