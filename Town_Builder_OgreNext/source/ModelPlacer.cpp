#include "ModelPlacer.h"

void CModelPlacer::Update()
{
	if (mAttachToMouse)
	{
		mEntNode->setVisible(true);

		SceneNode* cam = mSceneManager->getSceneNode("camera");
		Ray ray(cam->_getDerivedPosition(), cam->_getDerivedOrientation() * -Vector3::UNIT_Z);

		TerrainGroup::RayResult result = mTerrain->getTerrainGroup()->rayIntersects(ray);
		if (result.hit)
		{


			float heightDelta = CalculateHeightDelta(result.position);
			if (heightDelta > -15.0f && heightDelta < 15.0f)
			{
				if (mTintState != GREEN)
				{
					TintObject(mEntity, 0.5f, 1.0f, 0.01f, 0.65f);
					mTintState = GREEN;
				}
			}
			else
			{
				if (mTintState != RED)
				{
					TintObject(mEntity, 1.0f, 0.0f, 0.01f, 0.65f);
					mTintState = RED;
				}
			}

			//std::cout << "Ray hitted! at position: " << result.position << std::endl;
			AxisAlignedBox aabb = mEntNode->_getWorldAABB();
			Vector3 delta = (aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_TOP) - aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_BOTTOM)) * 0.5;
			mEntNode->setPosition(result.position /* + delta*/);
			Quaternion q;
			q.FromAngleAxis(cam->_getDerivedOrientation().getYaw(), Vector3::UNIT_Y);
			mEntNode->setOrientation(q);

			mEntNode->_update(true, false);
			mEntNode->showBoundingBox(true);
		}


		//mSceneManager->getRootSceneNode()->addChild(entNode);
		//entNode->scale(Vector3(20, 20, 20));

	}
	else
	{
		mEntNode->setVisible(false);
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

			StringStream ss;
			ss << "House";
			ss << mPlacedObjects.size();

			Entity* entity = mSceneManager->createEntity("house.mesh");
			CloneAndApplyMaterial(entity);
			TintObject(entity, 1, 1, 1, 1.0f);
			SceneNode* sn = mSceneManager->getRootSceneNode()->createChildSceneNode();
			sn->attachObject(entity);
			sn->scale(Vector3(10, 10, 10));

			std::cout << "Placed at position: " << result.position << std::endl;
			AxisAlignedBox aabb = sn->_getWorldAABB();
			Vector3 delta = (aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_TOP) - aabb.getCorner(AxisAlignedBox::CornerEnum::NEAR_LEFT_BOTTOM)) * 0.5;
			sn->setPosition(result.position /* + delta*/);
			Quaternion q;
			q.FromAngleAxis(cam->_getDerivedOrientation().getYaw(), Vector3::UNIT_Y);
			sn->setOrientation(q);

			sn->_update(true, false);
			sn->showBoundingBox(true);

			mTerrain->FlattenTerrainUnderObject(sn);

			mPlacedObjects.push_back(sn);
		}

	}

}

void CModelPlacer::TintObject(Entity* ent, float r, float g, float b, float a)
{
	auto nMaxSubMesh = ent->getMesh()->getNumSubMeshes();
	for (int nSubMesh = 0; nSubMesh < nMaxSubMesh; nSubMesh++)
	{
		auto matName = ent->getMesh()->getSubMesh(nSubMesh)->getMaterialName(); // MaterialName is an Ogre::String
		mMatPtr = MaterialManager::getSingleton().getByName(matName); // pMaterial is an Ogre::MaterialPtr
		std::cout << "mat: " << mMatPtr << std::endl;
		mMatPtr->getTechnique(0)->getPass(0)->setTransparentSortingEnabled(true);
		mMatPtr->getTechnique(0)->getPass(0)->setTransparentSortingForced(true);
		mMatPtr->getTechnique(0)->getPass(0)->setDiffuse(r, g, b, a);
	}
	
}

void CModelPlacer::CloneAndApplyMaterial(Entity * ent)
{
	auto nMaxSubMesh = ent->getMesh()->getNumSubMeshes();
	auto nMaxSubEnt = ent->getSubEntities();
	for (int nSubEnt = 0; nSubEnt < nMaxSubEnt.size(); nSubEnt++)
	{
		auto matName = ent->getSubEntity(nSubEnt)->getSubMesh()->getMaterialName();
		MaterialPtr matPtr = MaterialManager::getSingleton().getByName(matName);
		StringStream ss;
		ss << matName;
		ss << ent->getName();
		MaterialPtr clone = matPtr->clone(ss.str());

		ent->getSubEntity(nSubEnt)->setMaterial(clone);
		std::cout << "mat: " << matPtr << " clone: " << clone << " subent mat: " << ent->getSubEntity(nSubEnt)->getMaterial() << std::endl;
		//nMaxSubEnt[nSubEnt]->setMaterial(clone);
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

void CModelPlacer::InjectMousePress(const MouseButtonEvent& evt)
{
	if(evt.button == BUTTON_LEFT && mAttachToMouse)
	{
		PlaceObject();
	}
}

void CModelPlacer::InjectKeyDown(const KeyboardEvent& evt)
{
	if (evt.keysym.sym == SDLK_END)
	{
		mAttachToMouse = true;
	}
}
