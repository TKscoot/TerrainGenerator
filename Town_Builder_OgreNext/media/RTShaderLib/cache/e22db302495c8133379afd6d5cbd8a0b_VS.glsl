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
uniform	mat4	texture_matrix;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec4	uv0;
out	vec2	iTexcoord_0;
out	vec2	iTexcoord_1;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;

	FFP_Transform(worldviewproj_matrix, vertex, gl_Position);
	lLocalParam_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_1	=	vec4(0.000000,0.000000,0.000000,0.000000);
	iTexcoord_0	=	uv0.xy;
	FFP_TransformTexCoord(texture_matrix, iTexcoord_0, iTexcoord_0);
	iTexcoord_1	=	uv0.xy;
}

