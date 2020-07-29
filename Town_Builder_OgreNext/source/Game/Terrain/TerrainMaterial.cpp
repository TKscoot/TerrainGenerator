#include "TerrainMaterial.h"

TerrainMaterial::TerrainMaterial(Ogre::String materialName, bool addNormalmap, bool cloneMaterial)
	: mMaterialName(materialName), mAddNormalMap(addNormalmap), mCloneMaterial(cloneMaterial)
{
	mProfiles.push_back(OGRE_NEW SM2ProfileA(this, "OgreMaterial", "Profile for rendering Ogre standard material"));
	setActiveProfile("OgreMaterial");


	for (int i = 0; i < 3; i++)
	{
		StringStream ss;
		ss << "CovMap";
		ss << i;


		TextureManager::getSingleton().createManual(
			ss.str(), // name
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			TEX_TYPE_2D,      // type
			512, 512,         // width & height
			0,                // number of mipmaps
			PF_BYTE_RGBA,     // pixel format
			TU_DYNAMIC);      // usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
							  // textures updated very often (e.g. each frame);
	}
}

void TerrainMaterial::setMaterialByName(const Ogre::String materialName)
{
	mMaterialName = materialName;
	_markChanged();
}
// -----------------------------------------------------------------------------------------------------------------------


TerrainMaterial::SM2ProfileA::SM2ProfileA(TerrainMaterialGenerator* parent, const String& name, const String& desc) 
	: SM2Profile(parent, name, desc)
{
	HighLevelGpuProgramManager& hmgr = HighLevelGpuProgramManager::getSingleton();

}

TerrainMaterial::SM2ProfileA::~SM2ProfileA()
{
}

Ogre::MaterialPtr TerrainMaterial::SM2ProfileA::generate(const Ogre::Terrain * terrain)
{
		MaterialManager& matMgr = MaterialManager::getSingleton();
		TerrainMaterial* parent = (TerrainMaterial*)getParent();
		const String& matName = parent->mMaterialName;
		MaterialPtr mat = matMgr.getByName(matName);
		if (!mat)
		{
			mat = matMgr.create(matName, terrain->_getDerivedResourceGroup());
		}
		
		Pass* p = mat->getTechnique(0)->getPass(0);

		for (int i = 0; i < 3; i++)
		{
			StringStream ss;
			ss << "CovMap";
			ss << i;

			p->getTextureUnitState(i)->setTextureName(ss.str());
		}

	return mat;
}
