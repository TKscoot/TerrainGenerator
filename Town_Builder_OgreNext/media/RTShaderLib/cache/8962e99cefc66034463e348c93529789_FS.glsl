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
#include "FFPLib_Common.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	sampler2D	gTextureSampler0;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	iColor_0;
in	vec2	iTexcoord_0;
out vec4	oColor_0;
void main(void) {
	vec4	lLocalParam_0;
	vec4	texel_0;

	lLocalParam_0	=	vec4(0.000000,0.000000,0.000000,0.000000);
	oColor_0	=	iColor_0;
	texel_0	=	texture2D(gTextureSampler0, iTexcoord_0);
	oColor_0	=	texel_0*oColor_0;
	oColor_0.xyz	=	oColor_0.xyz+lLocalParam_0.xyz;
}

