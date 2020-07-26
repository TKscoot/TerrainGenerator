#include "TerrainMaterial.h"

TerrainMaterial::TerrainMaterial(Ogre::String materialName, bool addNormalmap, bool cloneMaterial)
	: mMaterialName(materialName), mAddNormalMap(addNormalmap), mCloneMaterial(cloneMaterial)
{
	mProfiles.push_back(OGRE_NEW SM2ProfileA(this, "OgreMaterial", "Profile for rendering Ogre standard material"));
	setActiveProfile("OgreMaterial");
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


//TerrainMaterial::Profile::Profile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc)
//	: Ogre::TerrainMaterialGenerator::Profile(parent, name, desc)
//{
//}
//
//TerrainMaterial::Profile::~Profile()
//{
//}
//
//Ogre::MaterialPtr TerrainMaterial::Profile::generate(const Ogre::Terrain* terrain)
//{
// 	const Ogre::String& matName = terrain->getMaterialName();
//
//	//Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
//	//if (!mat)
//	//	Ogre::MaterialManager::getSingleton().remove(matName);
//	TerrainMaterial* parent = (TerrainMaterial*)getParent();
//	
//	// Set Ogre material 
//	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(parent->mMaterialName);
//	
//	if (!mat)
//	{
//		std::cout << "MATERIAL IS NULL!" << std::endl;
//	}
//
//	// Clone material
//	if (parent->mCloneMaterial)
//	{
//		mat = mat->clone(matName);
//		parent->mMaterialName = matName;
//	}
//	
//	// Add normalmap
//	if (parent->mAddNormalMap)
//	{
//		// Get default pass
//		Ogre::Pass *p = mat->getTechnique(0)->getPass(0);
//	
//		// Add terrain's global normalmap to renderpass so the fragment program can find it.
//		Ogre::TextureUnitState *tu = p->createTextureUnitState(matName + "/nm");
//	
//		Ogre::TexturePtr nmtx = terrain->getTerrainNormalMap();
//		tu->_setTexturePtr(nmtx);
//	}
//
//	return mat;
//}
//
//Ogre::MaterialPtr TerrainMaterial::Profile::generateForCompositeMap(const Ogre::Terrain* terrain)
//{
//	Ogre::MaterialPtr mat = terrain->_getCompositeMapMaterial();
//	if (!mat)
//	{
//		std::cout << "COMPOSITE MATERIAL IS NULL!" << std::endl;
//	}
//
//	return mat;
//}
//
//Ogre::uint8 TerrainMaterial::Profile::getMaxLayers(const Ogre::Terrain* terrain) const
//{
//	return 0;
//}
//
//void TerrainMaterial::Profile::updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain)
//{
//}
//
//void TerrainMaterial::Profile::updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain)
//{
//}
//
//void TerrainMaterial::Profile::requestOptions(Ogre::Terrain* terrain)
//{
//	terrain->_setMorphRequired(false);
//	terrain->_setNormalMapRequired(true); // enable global normal map
//	terrain->_setLightMapRequired(false);
//	terrain->_setCompositeMapRequired(false);
//}
//
//void TerrainMaterial::Profile::setLightmapEnabled(bool enabled)
//{
//}

Ogre::MaterialPtr TerrainMaterial::SM2ProfileA::generate(const Ogre::Terrain * terrain)
{
	// re-use old material if exists
	//MaterialPtr mat = terrain->_getMaterial();
	//if (!mat)
	//{
		MaterialManager& matMgr = MaterialManager::getSingleton();
		TerrainMaterial* parent = (TerrainMaterial*)getParent();
		const String& matName = parent->mMaterialName;
		MaterialPtr mat = matMgr.getByName(matName);
		if (!mat)
		{
			mat = matMgr.create(matName, terrain->_getDerivedResourceGroup());
		}
		
	//}
	// clear everything
	//mat->removeAllTechniques();
	//
	//// Automatically disable normal & parallax mapping if card cannot handle it
	//// We do this rather than having a specific technique for it since it's simpler
	//GpuProgramManager& gmgr = GpuProgramManager::getSingleton();
	//if (!gmgr.isSyntaxSupported("ps_4_0") && !gmgr.isSyntaxSupported("ps_3_0") && !gmgr.isSyntaxSupported("ps_2_x")
	//	&& !gmgr.isSyntaxSupported("fp40") && !gmgr.isSyntaxSupported("arbfp1") && !gmgr.isSyntaxSupported("glsl")
	//	&& !gmgr.isSyntaxSupported("glsles"))
	//{
	//	setLayerNormalMappingEnabled(false);
	//	setLayerParallaxMappingEnabled(false);
	//}
	//
	//addTechnique(mat, terrain, HIGH_LOD);
	//
	//// LOD
	//if (mCompositeMapEnabled)
	//{
	//	addTechnique(mat, terrain, LOW_LOD);
	//	Material::LodValueList lodValues;
	//	lodValues.push_back(TerrainGlobalOptions::getSingleton().getCompositeMapDistance());
	//	mat->setLodLevels(lodValues);
	//	Technique* lowLodTechnique = mat->getTechnique(1);
	//	lowLodTechnique->setLodIndex(1);
	//}
	//
	//updateParams(mat, terrain);

	return mat;
}
