#include "pch.h"
#include "PlantPlacer.h"
#include "HeightFunction.h"


void CPlantPlacer::Initialize()
{
	
	PlaceTrees();
	PlaceGrass();
}

void CPlantPlacer::PlaceTrees()
{
	mTreesPG->setCamera(mSceneManager->getCamera("Camera"));

	mTreesPG->setPageSize(150);
	mTreesPG->setInfinite();

	mTreesPG->addDetailLevel<BatchPage>(1500, 30);
	mTreesPG->addDetailLevel<ImpostorPage>(4000, 50);

	AxisAlignedBox terrainAABB = mTerrain->getTerrainGroup()->getTerrain(0, 0)->getWorldAABB();
	TBounds bounds = mTreesPG->convertAABToTBounds(terrainAABB);

	mTreeLoader = new TreeLoader3D(mTreesPG, bounds);
	mTreesPG->setPageLoader(mTreeLoader);

	mEntity = mSceneManager->createEntity("Tree", "tree.mesh");

	HeightFunction::initialize(mTerrain->getTerrainGroup());

	Vector3 position;
	Radian yaw;
	Real scale;

	for (int x = bounds.left; x < bounds.right; x++)
	{
		for (int z = bounds.left; z < bounds.right; z++)
		{

			position.y = mTerrain->getTerrainGroup()->getHeightAtWorldPosition(position);


			if (position.y > 5 && position.y < 200)
			{
				int rand = Math::RangeRandom(0, 10);
				if (rand == 10)
				{
					position.x = x;
					position.z = z;
					
					yaw = Degree(Math::RangeRandom(0, 360));
					//scale = Math::RangeRandom(150.5f, 150.6f);
					scale = 10.0f;

					mTreeLoader->addTree(mEntity, position, yaw, scale);
				}

			}

		}

	}
}

void CPlantPlacer::PlaceGrass()
{
	mGrassPG->setCamera(mSceneManager->getCamera("Camera"));
	mGrassPG->setPageSize(150);
	mGrassPG->setInfinite();
	mGrassPG->addDetailLevel<GrassPage>(1500);

	GrassLoader* grassLoader = new GrassLoader(mGrassPG);
	mGrassPG->setPageLoader(grassLoader);
	grassLoader->setHeightFunction(&HeightFunction::getTerrainHeight);

	GrassLayer* layer = grassLoader->addLayer("grass");
	layer->setHeightRange(20.0f, 180.0f);
	layer->setMinimumSize(5.0f, 5.0f);
	layer->setMaximumSize(5.5f, 5.5f);
	layer->setAnimationEnabled(false);
	layer->setSwayDistribution(10.0f);
	layer->setSwayLength(0.5f);
	layer->setSwaySpeed(0.5f);
	//layer->setDensity(0.4f);
	layer->setFadeTechnique(FADETECH_GROW);
	layer->setDensityMap("terrain.png", Forests::CHANNEL_COLOR);

	AxisAlignedBox terrainAABB = mTerrain->getTerrainGroup()->getTerrain(0, 0)->getWorldAABB();
	TBounds bounds = mTreesPG->convertAABToTBounds(terrainAABB);

	layer->setMapBounds(bounds);
	layer->setColorMap(mTerrain->getColorMap());
	layer->setMaxSlope(Degree(5.0f).valueRadians());
}

void CPlantPlacer::Update()
{
	mTreesPG->update();
	mGrassPG->update();
}
