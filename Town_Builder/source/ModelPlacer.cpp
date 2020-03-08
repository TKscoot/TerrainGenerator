#include "pch.h"
#include "ModelPlacer.h"

void CModelPlacer::Update()
{
	if (mAttachToMouse)
	{
		SceneNode* cam = mSceneManager->getSceneNode("camera");
		Ray ray(cam->_getDerivedPosition(), cam->_getDerivedOrientation() * -Vector3::UNIT_Z);

		TerrainGroup::RayResult result = mTerrain->getTerrainGroup()->rayIntersects(ray);
		if (result.hit)
		{


			float heightDelta = CalculateHeightDelta(result.position);
			if (heightDelta > -15.0f && heightDelta < 15.0f)
			{
				TintObject(0.5f, 1.0f, 0.01f, 0.65f);
			}
			else
			{
				TintObject(1.0f, 0.0f, 0.01f, 0.65f);
			}



			//std::cout << "Ray hitted! at position: " << result.position << std::endl;
			AxisAlignedBox aabb = mEntNode->_getWorldAABB();
			Vector3 delta = (aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_TOP) - aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_BOTTOM)) * 0.5;
			mEntNode->setPosition(result.position + delta);
			Quaternion q;
			q.FromAngleAxis(cam->_getDerivedOrientation().getYaw(), Vector3::UNIT_Y);
			mEntNode->setOrientation(q);

			mEntNode->_update(true, false);
			mEntNode->showBoundingBox(true);
		}


		//mSceneManager->getRootSceneNode()->addChild(entNode);
		//entNode->scale(Vector3(20, 20, 20));

	}

}

void CModelPlacer::PlaceObject()
{
	SceneNode* cam = mSceneManager->getSceneNode("camera");
	Ray ray(cam->_getDerivedPosition(), cam->_getDerivedOrientation() * -Vector3::UNIT_Z);

	TerrainGroup::RayResult result = mTerrain->getTerrainGroup()->rayIntersects(ray);
	if (result.hit)
	{

		float heightDelta = CalculateHeightDelta(result.position);
		if (heightDelta > -15.0f && heightDelta < 15.0f)
		{
			mAttachToMouse = false;


			TintObject(1, 1, 1, 1.0f);

			std::cout << "Placed at position: " << result.position << std::endl;
			AxisAlignedBox aabb = mEntNode->_getWorldAABB();
			Vector3 delta = (aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_TOP) - aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_BOTTOM)) * 0.5;
			mEntNode->setPosition(result.position + delta);

			mEntNode->_update(true, false);
			mEntNode->showBoundingBox(true);

			mTerrain->FlattenTerrainUnderObject(mEntNode);
		}

	}

}

void CModelPlacer::TintObject(float r, float g, float b, float a)
{
	auto nMaxSubMesh = mEntity->getMesh()->getNumSubMeshes();
	for (int nSubMesh = 0; nSubMesh < nMaxSubMesh; nSubMesh++)
	{
		auto MaterialName = mEntity->getMesh()->getSubMesh(nSubMesh)->getMaterialName(); // MaterialName is an Ogre::String
		mMatPtr = MaterialManager::getSingleton().getByName(MaterialName); // pMaterial is an Ogre::MaterialPtr

		mMatPtr->getTechnique(0)->getPass(0)->setTransparentSortingEnabled(true);
		mMatPtr->getTechnique(0)->getPass(0)->setTransparentSortingForced(true);
		mMatPtr->getTechnique(0)->getPass(0)->setDiffuse(r, g, b, a);
	}
	
}

float CModelPlacer::CalculateHeightDelta(Vector3 position)
{

	Vector3 pos1 = mTerrain->getTerrainGroup()->getTerrain(0, 0)->convertPosition(Terrain::Space::WORLD_SPACE, position, Terrain::Space::POINT_SPACE);
	Vector3 pos2 = pos1 + 5.0f;

	float h1 = mTerrain->getTerrainGroup()->getTerrain(0, 0)->getHeightAtPoint(pos1.x, pos1.y);
	float h2 = mTerrain->getTerrainGroup()->getTerrain(0, 0)->getHeightAtPoint(pos2.x, pos2.y);

	float hd = h1 - h2;

	//std::cout << "Height delta: " << hd << std::endl;
	return hd;
}

void CModelPlacer::InjectMousePress(const OIS::MouseEvent & arg, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left && mAttachToMouse)
	{
		PlaceObject();
	}
}

void CModelPlacer::InjectKeyDown(const OIS::KeyEvent & evt)
{
	if (evt.key == OIS::KC_B)
	{
		mAttachToMouse = true;
	}
}
