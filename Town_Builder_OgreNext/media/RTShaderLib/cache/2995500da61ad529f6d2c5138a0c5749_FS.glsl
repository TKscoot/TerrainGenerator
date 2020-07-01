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
#include "SGXLib_TriplanarTexturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	sampler2D	tp_sampler_from_x0;
uniform	sampler2D	tp_sampler_from_y1;
uniform	sampler2D	tp_sampler_from_z2;
uniform	vec3	gTPParams0;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	iColor_0;
in	vec3	iTexcoord_0;
in	vec4	iTexcoord_1;
out vec4	oColor_0;
void main(void) {
	vec4	lLocalParam_0;

	lLocalParam_0	=	vec4(0.000000,0.000000,0.000000,0.000000);
	oColor_0	=	iColor_0;
	SGX_TriplanarTexturing(oColor_0, iTexcoord_0, iTexcoord_1, tp_sampler_from_x0, tp_sampler_from_y1, tp_sampler_from_z2, gTPParams0, oColor_0);
	oColor_0.xyz	=	oColor_0.xyz+lLocalParam_0.xyz;
}

