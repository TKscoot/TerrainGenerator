#pragma once
#include <Ogre.h>

using namespace Ogre;

class Erosion
{
public:
	Erosion(std::vector<float> map, int size)
		: mSize(size)

	{
		CEvent::GetSingletonPtr()->AddFrameStartedCallback(std::bind(&Erosion::Update, this, std::placeholders::_1));

	}

	bool Update(const FrameEvent& evt)
	{
		ImGui::BeginChild("Erosion", ImVec2(0, 350), true);

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
		float speed				  = 1.0f;
		float water				  = 1.0f;
		float sediment			  = 0.0f;
		
		// Choose random position on terrain
		pos.x = rand() % (mSize - 1);
		pos.y = rand() % (mSize - 1);

		for (int lifetime = 0; lifetime < 30; lifetime++)
		{

			int nodeX = (int)pos.x;
			int nodeY = (int)pos.y;
			int dropletIndex = nodeY * mSize + nodeX;
			// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
			float cellOffsetX = pos.x - nodeX;
			float cellOffsetY = pos.y - nodeY;

			// Calculating Gradients
			int index = pos.y * mSize + pos.x;
			index = std::max(index, mSize - 1);
			float h1 = mMap[index];
			float h2 = mMap[index + 1];
			float h3 = mMap[index + mSize];
			float h4 = mMap[index + 1 + mSize];

			Vector2 gradient;
			gradient.x = (h2 - h1) * (1 - pos.y) + (h4 - h3) * pos.y;
			gradient.y = (h3 - h1) * (1 - pos.x) + (h4 - h2) * pos.x;

			int coordX = (int)pos.x;
			int coordY = (int)pos.y;
			float x = pos.x - coordX;
			float y = pos.y - coordY;

			float height = h1 * (1 - x) * (1 - y) + h2 * x * (1 - y) + h3 * (1 - x) * y + h4 * x * y;


			// Calculating new Direction
			Vector2 newDir = direction * mInertia - gradient * (1 - mInertia);
			newDir = newDir.normalisedCopy();

			// Calculating new Position
			Vector2 newPos = pos + newDir;

			// Stop simulating droplet if it's not moving or has flowed over edge of map
			if ((newDir.x == 0.0f && newDir.y == 0.0f) || newPos.x <= 0 || newPos.x >= mSize - 1 || newPos.y <= 0 || newPos.y >= mSize - 1)
			{
				return;
			}

			// Calculating new and delta height
			index = newPos.y * mSize + newPos.x;
			index = std::max(index, mSize - 1);
			float heightNW = mMap[index];
			float heightNE = mMap[index + 1];
			float heightSW = mMap[index + mSize];
			float heightSE = mMap[index + 1 + mSize];

			coordX = (int)newPos.x;
			coordY = (int)newPos.y;
			x = newPos.x - coordX;
			y = newPos.y - coordY;

			float newHeight = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
			float deltaHeight = newHeight - height;

			float carryCapacity = std::max(-deltaHeight, mMinSlope) /** speed*/ * water * mMaxSedimentCapacity;

			// Lagues berechnung
			//float carryCapacity = std::max(-deltaHeight * speed * water * mMaxSedimentCapacity, mMinSlope);
			
			if (deltaHeight < 0)
			{
				if (sediment > carryCapacity)
				{
					float liegelassSediment = (sediment - carryCapacity) * mDeposition;
					sediment += liegelassSediment;
					//mMap[pos.x * mSize + pos.y] += liegelassSediment;

					float idx = pos.x * mSize + pos.y;
					//float idx = std::max(index, mSize - 1);
					mMap[idx] += liegelassSediment;
					mMap[idx + 1] += liegelassSediment;
					mMap[idx + mSize] += liegelassSediment;
					mMap[idx + 1 + mSize] += liegelassSediment;

				}
				else
				{
					float mitnehmSediment = std::min((carryCapacity - sediment)*mErosion, -deltaHeight);
					sediment -= mitnehmSediment;
					mMap[pos.x * mSize + pos.y] -= mitnehmSediment;
				}
			}

			mMap[pos.x * mSize + pos.y] = std::max(mMap[pos.x * mSize + pos.y], 0.0f);

			// Update droplet's speed and water content

			speed += std::sqrt(speed * speed + deltaHeight * mGravity);


			water *= (1 - mEvaporationSpeed);

			if(water <= 0)
			{
				return;
			}

			pos = newPos;
		}
		
		return;
	}


private:

	std::vector<float> mMap;
	int mSize;

	float mInertia			   = 0.05f;
	float mInitialSpeed		   = 1.0f;
	float mInitialWater		   = 1.0f;
	float mInitialSediment	   = 0.0f;
	float mMinSlope			   = 0.1f;
	float mMaxSedimentCapacity = 1.0f;
	float mDeposition		   = 0.9f;
	float mErosion		       = 0.01f;
	float mGravity			   = 4.0f;
	float mEvaporationSpeed	   = 0.01f;

};