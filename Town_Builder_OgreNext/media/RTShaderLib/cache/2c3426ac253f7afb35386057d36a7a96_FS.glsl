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

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec3	iTexcoord_0;
out vec4	oColor_0;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;

	lLocalParam_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_1	=	vec4(0.000000,0.000000,0.000000,0.000000);
	oColor_0	=	lLocalParam_0;
	oColor_0	=	derived_scene_colour;
	lLocalParam_0	=	oColor_0;
	oColor_0.xyz	=	oColor_0.xyz+lLocalParam_1.xyz;
}

