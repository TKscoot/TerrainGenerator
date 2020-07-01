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
#include "FFPLib_Transform.glsl"
#include "FFPLib_Common.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat4	worldviewproj_matrix;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec4	colour;
in	vec4	uv0;
out	vec4	iColor_0;
out	vec2	iTexcoord_0;
void main(void) {
	vec4	lLocalParam_0;

	FFP_Transform(worldviewproj_matrix, vertex, gl_Position);
	iColor_0	=	colour;
	lLocalParam_0	=	vec4(0.000000,0.000000,0.000000,0.000000);
	iTexcoord_0	=	uv0.xy;
}

