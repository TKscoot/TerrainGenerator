#pragma once
#include <tuple>
#include <Ogre.h>
#include "Engine/Event.h"
#include "imgui.h"

using namespace Ogre;

class Erosion
{
public:

	struct HeightAndGradient
	{
		float height;
		float gradientX;
		float gradientY;
	};



	Erosion(int size)
		: mSize(size)

	{
		CEvent::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&Erosion::Update, this, std::placeholders::_1));

	}

	bool Update(const FrameEvent& evt)
	{
		ImGui::BeginChild("Erosion", ImVec2(0, 350), true);

		ImGui::Text("Current speed:    %.4f", mSpeed);
		ImGui::Text("Current water:    %.4f", mWater);
		ImGui::Text("Current sediment: %.4f", mSediment);

		ImGui::SliderFloat("Inertia", &mInertia, 0.0f, 1.0f);
		ImGui::SliderFloat("InitialSpeed", &mInitialSpeed, 0.0f, 20.0f);
		ImGui::SliderFloat("InitialWater", &mInitialWater, 0.0f, 20.0f);
		ImGui::SliderFloat("InitialSediment", &mInitialSediment, 0.0f, 1.0f);
		ImGui::SliderFloat("MinSlope", &mMinSlope, 0.0f, 20.0f);
		ImGui::SliderFloat("MaxSedimentCapacity", &mMaxSedimentCapacity, 1.0f, 64.0f);
		ImGui::SliderFloat("Deposition", &mDeposition, 0.0f, 1.0f);
		ImGui::SliderFloat("Erosion", &mErosion, 0.0f, 1.0f);
		ImGui::SliderFloat("Gravity", &mGravity, 1.0f, 10.0f);
		ImGui::SliderFloat("EvaporationSpeed", &mEvaporationSpeed, 0.0f, 1.0f);

		ImGui::EndChild();
		ImGui::End();

		return true;
	}


	void Calculate(std::vector<float> &mMap)
	{
		// Instantiating droplet
		Vector2 pos				  = Vector2::ZERO;
		Vector2 direction		  = Vector2::ZERO;
		mSpeed				      = 1.0f;
		mWater				      = 1.0f;
		mSediment			      = 0.0f;
		
		// Choose random position on terrain
		pos.x = rand() % (mSize - 1);
		pos.y = rand() % (mSize - 1);

		for (int lifetime = 0; lifetime < mMaxDropletLifetime; lifetime++)
		{

			int nodeX = (int)pos.x;
			int nodeY = (int)pos.y;
			int dropletIndex = nodeY * mSize + nodeX;
			// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
			float cellOffsetX = pos.x - nodeX;
			float cellOffsetY = pos.y - nodeY;

			
			// Calculating Gradients and Height
			Vector2 gradient = Vector2::ZERO;
			float height = 0.0f;

			{
				int coordX = (int)pos.x;
				int coordY = (int)pos.y;

				// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
				float x = pos.x - coordX;
				float y = pos.y - coordY;

				// Calculate heights of the four nodes of the droplet's cell
				int nodeIndexNW = coordY * mSize + coordX;
				float heightNW = mMap[nodeIndexNW];
				float heightNE = mMap[nodeIndexNW + 1];
				float heightSW = mMap[nodeIndexNW + mSize];
				float heightSE = mMap[nodeIndexNW + mSize + 1];

				// Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges

				gradient = Vector2(
					(heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y,
					(heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x);

				// Calculate height with bilinear interpolation of the heights of the nodes of the cell
				height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
			}

			// Calculating new Direction
			Vector2 newDir = (direction * mInertia - gradient * (1 - mInertia));
			// Normalize direction
			float len = sqrt(newDir.x * newDir.x + newDir.y * newDir.y);
			if (len != 0)
			{
				newDir.x /= len;
				newDir.y /= len;
			}

			//newDir = newDir.normalisedCopy();

			// Calculating new Position
			Vector2 newPos = pos + newDir;

			// Stop simulating droplet if it's not moving or has flowed over edge of map
			if ((newDir.x == 0.0f && newDir.y == 0.0f) || newPos.x <= 0 || newPos.x >= mSize - 1 || newPos.y <= 0 || newPos.y >= mSize - 1)
			{
				return;
			}

			// Calculating new Height
			float newHeight = 0.0f;

			{
				int coordX = (int)newPos.x;
				int coordY = (int)newPos.y;

				// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
				float x = newPos.x - coordX;
				float y = newPos.y - coordY;

				// Calculate heights of the four nodes of the droplet's cell
				int nodeIndexNW = coordY * mSize + coordX;
				float heightNW = mMap[nodeIndexNW];
				float heightNE = mMap[nodeIndexNW + 1];
				float heightSW = mMap[nodeIndexNW + mSize];
				float heightSE = mMap[nodeIndexNW + mSize + 1];

				// Calculate height with bilinear interpolation of the heights of the nodes of the cell
				newHeight = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
			}

			float deltaHeight = newHeight - height;

			float carryCapacity = std::max(-deltaHeight, mMinSlope) /** mSpeed*/ * mWater * mMaxSedimentCapacity;

			// Lagues berechnung
			//float carryCapacity = std::max(-deltaHeight * mSpeed * mWater * mMaxSedimentCapacity, mMinSlope);
			

			if (mSediment > carryCapacity || deltaHeight > 0)
			{

				//float depositionSediment = (mSediment - carryCapacity) * mDeposition;
				float depositionSediment = (deltaHeight > 0) ? std::min(deltaHeight, mSediment) : (mSediment - carryCapacity) * mDeposition;
				mSediment -= depositionSediment;

				mMap[dropletIndex]			   += depositionSediment * (1 - cellOffsetX) * (1 - cellOffsetY);
				mMap[dropletIndex + 1]		   += depositionSediment *		cellOffsetX  * (1 - cellOffsetY);
				mMap[dropletIndex + mSize]     += depositionSediment * (1 - cellOffsetX) *		cellOffsetY ;
				mMap[dropletIndex + mSize + 1] += depositionSediment *		cellOffsetX  *		cellOffsetY ;
			}
			else
			{
				float erosionSediment = std::min((carryCapacity - mSediment) * mErosion, -deltaHeight);
				mMap[dropletIndex] -= erosionSediment;

				mSediment += erosionSediment;
			}
			

			mMap[dropletIndex] = std::max(mMap[dropletIndex], 0.0f);

			// Update droplet's speed and water content
			mSpeed = std::sqrt(mSpeed * mSpeed + deltaHeight * mGravity);
			mWater *= (1 - mEvaporationSpeed);

			if(mWater <= 0)
			{
				return;
			}

			pos = newPos;
		}
		
		return;
	}


	std::tuple<Vector2, float> CalculateHeightAndGradient(std::vector<float> nodes, int mapSize, float posX, float posY)
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

		//HeightAndGradient hag

		return {Vector2(gradientX, gradientY), height };
	}

private:

	std::vector<float> mMap;
	int mSize;

	int   mMaxDropletLifetime  = 30;
	float mInertia			   = 0.05f;
	float mInitialSpeed		   = 1.0f;
	float mInitialWater		   = 1.0f;
	float mInitialSediment	   = 0.0f;
	float mMinSlope			   = 0.1f;
	float mMaxSedimentCapacity = 1.0f;
	float mDeposition		   = 0.01f;
	float mErosion		       = 0.01f;
	float mGravity			   = 4.0f;
	float mEvaporationSpeed	   = 0.01f;

	// current vars (members, so i can use them with ImGui)
	float mSpeed    = 1.0f;
	float mWater    = 1.0f;
	float mSediment = 0.0f;


	HeightAndGradient hag;
};