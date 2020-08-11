#include "TerrainMaterial.h"

TerrainMaterial::TerrainMaterial(Ogre::String materialName)
	: mMaterialName(materialName)
{
	mProfiles.push_back(OGRE_NEW SM2ProfileA(this, "OgreMaterial", "Profile for rendering Ogre standard material"));
	setActiveProfile("OgreMaterial");

	// Create coverage map textures
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
			TU_DYNAMIC);      // usage flags
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

		// Set texture samplers for shaders
		for (int i = 0; i < 3; i++)
		{
			StringStream ss;
			ss << "CovMap";
			ss << i;

			p->getTextureUnitState(i)->setTextureName(ss.str());
		}

		SamplerPtr mapSampler = TextureManager::getSingleton().createSampler();
		mapSampler->setAddressingMode(TAM_CLAMP);

		TextureUnitState* tu = p->getTextureUnitStates()[10];
		tu->setTexture(terrain->getTerrainNormalMap());
		tu->setSampler(mapSampler);

		// Set shader params
		p->getFragmentProgram()->createParameters()->setNamedAutoConstant(
			"lightDiffuse", 
			Ogre::GpuProgramParameters::AutoConstantType::ACT_LIGHT_DIFFUSE_COLOUR, 0);

		p->getFragmentProgram()->createParameters()->setNamedAutoConstant(
			"lightSpecular", 
			Ogre::GpuProgramParameters::AutoConstantType::ACT_LIGHT_SPECULAR_COLOUR, 0);

		p->getFragmentProgram()->createParameters()->setNamedAutoConstant(
			"lightPosition", 
			Ogre::GpuProgramParameters::AutoConstantType::ACT_LIGHT_POSITION_OBJECT_SPACE, 0);

		p->getFragmentProgram()->createParameters()->setNamedAutoConstant(
			"eyePosition", 
			Ogre::GpuProgramParameters::AutoConstantType::ACT_CAMERA_POSITION_OBJECT_SPACE);

		p->getFragmentProgram()->createParameters()->setNamedAutoConstant(
			"attenuation", 
			Ogre::GpuProgramParameters::AutoConstantType::ACT_LIGHT_ATTENUATION, 0);

		p->getFragmentProgram()->createParameters()->setNamedAutoConstant(
			"ambient", 
			Ogre::GpuProgramParameters::AutoConstantType::ACT_AMBIENT_LIGHT_COLOUR, 0);

	return mat;
}

void TerrainMaterial::SM2ProfileA::requestOptions(Ogre::Terrain* terrain)
{
	terrain->_setMorphRequired(true);
	terrain->_setNormalMapRequired(true);
	terrain->_setLightMapRequired(mLightmapEnabled, true);
	terrain->_setCompositeMapRequired(mCompositeMapEnabled);
}

