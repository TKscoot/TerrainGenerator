#version 460
#define texture1D texture
#define texture2D texture
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#include "FFPLib_Texturing.glsl"
#include "SGXLib_PerPixelLighting.glsl"
#include "SGXLib_NormalMap.glsl"
#include "FFPLib_Common.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	sampler2D	gNormalMapSampler1;
uniform	vec4	derived_ambient_light_colour;
uniform	vec4	surface_emissive_colour;
uniform	vec4	derived_scene_colour;
uniform	float	surface_shininess;
uniform	vec4	light_attenuation;
uniform	vec4	derived_light_diffuse_colour;
uniform	vec4	derived_light_specular_colour;
uniform	vec4	derived_light_diffuse_colour1;
uniform	vec4	derived_light_specular_colour1;
uniform	sampler2D	gTextureSampler0;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec2	iTexcoord_0;
in	vec3	iTexcoord_1;
in	vec3	iTexcoord_2;
in	vec3	iTexcoord_3;
out vec4	oColor_0;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;
	vec3	lLocalParam_2;
	vec4	texel_0;

	lLocalParam_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_1	=	vec4(0.000000,0.000000,0.000000,0.000000);
	oColor_0	=	lLocalParam_0;
	vec2 local_iTexcoord_0 = iTexcoord_0;
	SGX_Generate_Parallax_Texcoord(gNormalMapSampler1, local_iTexcoord_0, iTexcoord_1, vec2(0.0400000,-0.0200000), local_iTexcoord_0);
	local_iTexcoord_0	=	local_iTexcoord_0;
	SGX_FetchNormal(gNormalMapSampler1, local_iTexcoord_0, lLocalParam_2);
	oColor_0	=	derived_scene_colour;
	SGX_Light_Point_DiffuseSpecular(lLocalParam_2, iTexcoord_1, iTexcoord_2, light_attenuation, derived_light_diffuse_colour.xyz, derived_light_specular_colour.xyz, surface_shininess, oColor_0.xyz, lLocalParam_1.xyz);
	SGX_Light_Directional_DiffuseSpecular(lLocalParam_2, iTexcoord_1, iTexcoord_3.xyz, derived_light_diffuse_colour1.xyz, derived_light_specular_colour1.xyz, surface_shininess, oColor_0.xyz, lLocalParam_1.xyz);
	lLocalParam_0	=	oColor_0;
	texel_0	=	texture2D(gTextureSampler0, local_iTexcoord_0);
	oColor_0	=	texel_0*oColor_0;
	oColor_0.xyz	=	oColor_0.xyz+lLocalParam_1.xyz;
}

