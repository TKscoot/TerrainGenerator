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
#include "SGXLib_PerPixelLighting.glsl"
#include "FFPLib_Common.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	vec4	derived_ambient_light_colour;
uniform	vec4	surface_emissive_colour;
uniform	vec4	derived_scene_colour;
uniform	float	surface_shininess;
uniform	vec4	light_position_view_space;
uniform	vec4	light_attenuation;
uniform	vec4	derived_light_diffuse_colour;
uniform	vec4	light_position_view_space1;
uniform	vec4	derived_light_diffuse_colour1;
uniform	sampler2D	gTextureSampler0;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec3	iTexcoord_0;
in	vec3	iTexcoord_1;
in	vec2	iTexcoord_2;
out vec4	oColor_0;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;
	vec3	lLocalParam_2;
	vec3	lLocalParam_3;
	vec4	texel_0;

	lLocalParam_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_1	=	vec4(0.000000,0.000000,0.000000,0.000000);
	oColor_0	=	lLocalParam_0;
	oColor_0	=	derived_scene_colour;
	lLocalParam_3	=	vec3(-1.00000,-1.00000,-1.00000)*iTexcoord_1;
	lLocalParam_2	=	light_position_view_space.xyz-iTexcoord_1;
	SGX_Light_Point_Diffuse(iTexcoord_0, lLocalParam_2, light_attenuation, derived_light_diffuse_colour.xyz, oColor_0.xyz);
	SGX_Light_Directional_Diffuse(iTexcoord_0, light_position_view_space1.xyz, derived_light_diffuse_colour1.xyz, oColor_0.xyz);
	lLocalParam_0	=	oColor_0;
	texel_0	=	texture2D(gTextureSampler0, iTexcoord_2);
	oColor_0	=	texel_0*oColor_0;
	oColor_0.xyz	=	oColor_0.xyz+lLocalParam_1.xyz;
}

