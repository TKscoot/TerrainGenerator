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

uniform	sampler2D	gNormalMapSampler3;
uniform	vec4	derived_ambient_light_colour;
uniform	vec4	surface_emissive_colour;
uniform	vec4	derived_scene_colour;
uniform	float	surface_shininess;
uniform	vec4	derived_light_diffuse_colour;
uniform	vec4	derived_light_specular_colour;
uniform	sampler2D	gTextureSampler0;
uniform	samplerCube	gTextureSampler1;
uniform	sampler2D	gTextureSampler2;

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
	vec4	texel_1;
	vec4	texel_2;

	lLocalParam_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_1	=	vec4(0.000000,0.000000,0.000000,0.000000);
	oColor_0	=	lLocalParam_0;
	SGX_FetchNormal(gNormalMapSampler3, iTexcoord_0, lLocalParam_2);
	oColor_0	=	derived_scene_colour;
	SGX_Light_Directional_DiffuseSpecular(lLocalParam_2, iTexcoord_1, iTexcoord_2.xyz, derived_light_diffuse_colour.xyz, derived_light_specular_colour.xyz, surface_shininess, oColor_0.xyz, lLocalParam_1.xyz);
	lLocalParam_0	=	oColor_0;
	texel_0	=	texture2D(gTextureSampler0, iTexcoord_0);
	texel_1	=	textureCube(gTextureSampler1, iTexcoord_3);
	texel_2	=	texture2D(gTextureSampler2, iTexcoord_0);
	oColor_0	=	texel_0*oColor_0;
	FFP_Lerp(oColor_0.xyz, texel_1.xyz, 0.200000, oColor_0.xyz);
	oColor_0.w	=	texel_1.w*oColor_0.w;
	oColor_0.xyz	=	texel_2.xyz+oColor_0.xyz;
	oColor_0.w	=	texel_2.w*oColor_0.w;
	oColor_0.xyz	=	oColor_0.xyz+lLocalParam_1.xyz;
}

