#include "HydraulicErosion.h"

bool CHydraulicErosion::Update(const FrameEvent & evt)
{
	static int erosionTime = 0.0f;

	if (ImGui::CollapsingHeader("Erosion", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::BeginChild("Erosion",	  ImVec2(0, 420), true);
		ImGui::BeginChild("ErosionStats", ImVec2(0, 65),  true);
		ImGui::Text("Current speed:    %.4f", mSpeed);
		ImGui::Text("Current water:    %.4f", mWater);
		ImGui::Text("Current sediment: %.4f", mSediment);
		ImGui::EndChild();
		ImGui::Spacing();

		ImGui::SliderInt("Iterations", &mIterations, 1, 20000);
		ImGui::Spacing();

		ImGui::SliderInt(  "MaxDropletLifetime",  &mMaxDropletLifetime,    1,    100);
		ImGui::SliderFloat("Inertia",			  &mInertia,			 0.0f,  1.0f);
		ImGui::SliderFloat("InitialSpeed",		  &mInitialSpeed,		 0.0f, 20.0f);
		ImGui::SliderFloat("InitialWater",		  &mInitialWater,		 0.0f, 20.0f);
		ImGui::SliderFloat("InitialSediment",	  &mInitialSediment,	 0.0f,  1.0f);
		ImGui::SliderFloat("MinSlope",			  &mMinSlope,			 0.0f, 20.0f);
		ImGui::SliderFloat("MaxSedimentCapacity", &mMaxSedimentCapacity, 1.0f, 64.0f);
		ImGui::SliderFloat("Deposition",		  &mDeposition,			 0.0f,  1.0f);
		ImGui::SliderFloat("Erosion",			  &mErosion,			 0.0f,  1.0f);
		ImGui::SliderFloat("Gravity",			  &mGravity,			 1.0f, 10.0f);
		ImGui::SliderFloat("EvaporationSpeed",    &mEvaporationSpeed,	 0.0f,  1.0f);
		ImGui::Spacing();

		if (ImGui::Button("Erode!"))
		{
			auto start = std::chrono::steady_clock::now();

			mErodeFunction(mIterations);

			auto end = std::chrono::steady_clock::now();
			erosionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		}
		ImGui::SameLine();
		ImGui::Text("Calculation took %d milliseconds!", erosionTime);
		ImGui::Spacing();

		if (ImGui::Button("Reset parameters!"))
		{
			ResetParameters();
		}
		ImGui::EndChild();
	}
	ImGui::Spacing();

	return true;
}

void CHydraulicErosion::Calculate(std::vector<float>& map)
{
	// Instantiating droplet
	Vector2 pos		  = Vector2::ZERO;
	Vector2 direction = Vector2::ZERO;
	mSpeed			  = mInitialSpeed;
	mWater			  = mInitialWater;
	mSediment		  = mInitialSediment;

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
			float heightNW = map[nodeIndexNW];
			float heightNE = map[nodeIndexNW + 1];
			float heightSW = map[nodeIndexNW + mSize];
			float heightSE = map[nodeIndexNW + mSize + 1];

			// Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
			gradient = Vector2(
				(heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y,
				(heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x);

			// Calculate height with bilinear interpolation of the heights of the nodes of the cell
			height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
		}

		// Calculating new Direction
		Vector2 newDir = (direction * mInertia - gradient * (1 - mInertia));

		// Normalizing Direction
		float len = sqrt(newDir.x * newDir.x + newDir.y * newDir.y);
		if (len != 0)
		{
			newDir.x /= len;
			newDir.y /= len;
		}

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
			float heightNW = map[nodeIndexNW];
			float heightNE = map[nodeIndexNW + 1];
			float heightSW = map[nodeIndexNW + mSize];
			float heightSE = map[nodeIndexNW + mSize + 1];

			// Calculate height with bilinear interpolation of the heights of the nodes of the cell
			newHeight = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;
		}

		float deltaHeight = newHeight - height;

		float carryCapacity = std::max(-deltaHeight, mMinSlope) /** mSpeed*/ * mWater * mMaxSedimentCapacity;

		// Lagues berechnung
		//float carryCapacity = std::max(-deltaHeight * mSpeed * mWater * mMaxSedimentCapacity, mMinSlope);

		// When flowing uphill deposit sediment
		if (mSediment > carryCapacity || deltaHeight > 0)
		{
			// Calculate how much sediment should be deposited
			float depositionSediment = (deltaHeight > 0) ? std::min(deltaHeight, mSediment) : (mSediment - carryCapacity) * mDeposition;
			mSediment -= depositionSediment;

			map[dropletIndex]			  += depositionSediment * (1 - cellOffsetX) * (1 - cellOffsetY);
			map[dropletIndex + 1]		  += depositionSediment *	   cellOffsetX  * (1 - cellOffsetY);
			map[dropletIndex + mSize]	  += depositionSediment * (1 - cellOffsetX) *	   cellOffsetY;
			map[dropletIndex + mSize + 1] += depositionSediment *      cellOffsetX  *	   cellOffsetY;
		}
		// When flowing downhill erode sediment
		else
		{
			// Calculate how much sediment should be added to droplet and take it away from heightmap
			float erosionSediment = std::min((carryCapacity - mSediment) * mErosion, -deltaHeight);

			map[dropletIndex] -= erosionSediment;
			mSediment += erosionSediment;
		}


		map[dropletIndex] = std::max(map[dropletIndex], 0.0f);

		// Update droplet's speed and water content
		mSpeed = std::sqrt(mSpeed * mSpeed + deltaHeight * mGravity);
		mWater *= (1 - mEvaporationSpeed);

		if (mWater <= 0)
		{
			return;
		}

		// Setting new Position to be the old Position for next Iteration
		pos = newPos;
	}
}
