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
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat3x4	world_matrix_array_3x4[70];
uniform	mat4	inverse_world_matrix;
uniform	mat4	viewproj_matrix;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec3	normal;
in	vec4	blendIndices;
in	vec4	blendWeights;
void main(void) {
	vec4	lLocalParam_0;
	vec3	lLocalParam_1;
	vec4	TempVal4;
	vec3	TempVal3;
	vec4	lLocalParam_4;
	vec4	lLocalParam_5;

	FFP_Transform(world_matrix_array_3x4[int(blendIndices.x)], vertex, TempVal4.xyz);
	TempVal4.w	=	1.00000;
	TempVal4	=	TempVal4*blendWeights.x;
	lLocalParam_0	=	TempVal4;
	vec4 local_vertex = vertex;
	FFP_Transform(inverse_world_matrix, lLocalParam_0, local_vertex);
	FFP_Transform(viewproj_matrix, lLocalParam_0, gl_Position);
	FFP_Transform(world_matrix_array_3x4[int(blendIndices.x)], normal, TempVal3);
	TempVal3	=	TempVal3*blendWeights.x;
	lLocalParam_1	=	TempVal3;
	vec3 local_normal = normal;
	FFP_Transform(inverse_world_matrix, lLocalParam_1, local_normal);
	lLocalParam_4	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_5	=	vec4(0.000000,0.000000,0.000000,0.000000);
}

