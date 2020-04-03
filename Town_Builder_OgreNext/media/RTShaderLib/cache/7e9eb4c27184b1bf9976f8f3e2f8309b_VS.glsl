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
#include "FFPLib_Transform.glsl"
#include "SGXLib_DualQuaternion.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat4	world_matrix;
uniform	mat4	worldviewproj_matrix;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec3	normal;
void main(void) {
	vec3	BlendedPosition;
	vec3	lLocalParam_1;
	vec4	lLocalParam_2;
	vec4	lLocalParam_3;

	FFP_Transform(worldviewproj_matrix, vertex, gl_Position);
	FFP_Transform(world_matrix, normal, lLocalParam_1);
	lLocalParam_2	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_3	=	vec4(0.000000,0.000000,0.000000,0.000000);
}

