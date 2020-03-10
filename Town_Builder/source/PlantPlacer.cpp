#include "pch.h"
#include "PlantPlacer.h"


void CPlantPlacer::Initialize()
{
	mTreesPG->setCamera(mSceneManager->getCamera("Camera"));

	mTreesPG->setPageSize(50);
	mTreesPG->setInfinite();

	mTreesPG->addDetailLevel<BatchPage>(1500, 30);
	mTreesPG->addDetailLevel<ImpostorPage>(4000, 50);

	AxisAlignedBox terrainAABB = mTerrain->getTerrainGroup()->getTerrain(0, 0)->getWorldAABB();
	TBounds bounds = mTreesPG->convertAABToTBounds(terrainAABB);
	
	mTreeLoader = new TreeLoader3D(mTreesPG, bounds);
	mTreesPG->setPageLoader(mTreeLoader);

	mEntity = mSceneManager->createEntity("Tree", "tree2_upgraded.mesh");

	//Randomly place 20,000 copies of the tree on the terrain
	Vector3 position;
	Radian yaw;
	Real scale;
	for (int i = 0; i < 20000; i++)
	{
		yaw = Degree(Math::RangeRandom(0, 360));

		position.x = Math::RangeRandom(0, 1500);
		position.z = Math::RangeRandom(0, 1500);
		position.y = mTerrain->getTerrainGroup()->getHeightAtWorldPosition(position);

		scale = Math::RangeRandom(150.5f, 150.6f);

		mTreeLoader->addTree(mEntity, position, yaw, scale);

	}

}

void CPlantPlacer::Update()
{
	mTreesPG->update();
}
