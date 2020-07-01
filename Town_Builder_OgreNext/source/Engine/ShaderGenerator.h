#pragma once

#include <Ogre.h>
#include <RTShaderSystem\OgreRTShaderSystem.h>

using namespace Ogre;

/*##
## This class demonstrates basic usage of the RTShader system.
## It sub class the material manager listener class and when a target scheme callback
## is invoked with the shader generator scheme it tries to create an equivalent shader
## based technique based on the default technique of the given material.
##*/

class ShaderGeneratorTechniqueResolverListener : public MaterialManager::Listener
{
public:

	ShaderGeneratorTechniqueResolverListener(RTShader::ShaderGenerator* pShaderGenerator)
	{
		m_pShaderGenerator = pShaderGenerator;
	}

	/** This is the hook point where shader based technique will be created.
	It will be called whenever the material manager won't find appropriate technique
	that satisfy the target scheme name. If the scheme name is out target RT Shader System
	scheme name we will try to create shader generated technique for it.
	*/
	virtual Technique* handleSchemeNotFound(unsigned short schemeIndex,
		const String& schemeName, Material* originalMaterial, unsigned short lodIndex,
		const Renderable* rend)
	{
		Technique* generatedTech = NULL;

		std::cout << "---------------------------------" << std::endl;
		std::cout << "handleSchemeNotFound GOT CALLED!!" << std::endl;
		std::cout << "---------------------------------" << std::endl;
		// Case this is the default shader generator scheme.
		if (schemeName == RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
		{
			bool techniqueCreated;

			// Create shader generated technique for this material.
			techniqueCreated = m_pShaderGenerator->createShaderBasedTechnique(
				*originalMaterial,
				MaterialManager::DEFAULT_SCHEME_NAME,
				schemeName);

			// Case technique registration succeeded.
			if (techniqueCreated)
			{
				// Force creating the shaders for the generated technique.
				m_pShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());

				// Grab the generated technique.
				std::vector<Technique* > techniques = originalMaterial->getTechniques();

				for(auto &technique : techniques)
				{
					Technique* curTech = technique;

					if (curTech->getSchemeName() == schemeName)
					{
						generatedTech = curTech;
						break;
					}
				}
			}
		}

		return generatedTech;
	}

protected:
	RTShader::ShaderGenerator*	m_pShaderGenerator;			// The shader generator instance.		
};