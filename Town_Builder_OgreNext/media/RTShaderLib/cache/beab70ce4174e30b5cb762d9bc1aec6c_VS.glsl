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
#include "SGXLib_PerPixelLighting.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat2x4	world_dualquaternion_array_2x4[24];
uniform	mat4	inverse_world_matrix;
uniform	mat4	viewproj_matrix;
uniform	mat3	normal_matrix;
uniform	mat4	worldview_matrix;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec3	normal;
in	vec4	blendIndices;
in	vec4	blendWeights;
out	vec3	iTexcoord_0;
out	vec3	iTexcoord_1;
void main(void) {
	vec3	BlendedPosition;
	vec3	lLocalParam_1;
	mat2x4	worldMatrix;
	mat2x4	blendDQ;
	mat2x4	initialDQ;
	mat2x4	TempVal2x4;
	vec4	TempVal4;
	vec3	TempVal3;
	vec4	lLocalParam_8;
	vec4	lLocalParam_9;

	BlendedPosition	=	vertex.xyz;
	TempVal2x4	=	world_dualquaternion_array_2x4[int(blendIndices.x)];
	initialDQ	=	TempVal2x4;
	TempVal2x4	=	blendWeights.x*TempVal2x4;
	blendDQ	=	TempVal2x4;
	TempVal2x4	=	world_dualquaternion_array_2x4[int(blendIndices.y)];
	SGX_AntipodalityAdjustment(initialDQ, TempVal2x4, TempVal2x4);
	TempVal2x4	=	blendWeights.y*TempVal2x4;
	blendDQ	=	TempVal2x4+blendDQ;
	TempVal2x4	=	world_dualquaternion_array_2x4[int(blendIndices.z)];
	SGX_AntipodalityAdjustment(initialDQ, TempVal2x4, TempVal2x4);
	TempVal2x4	=	blendWeights.z*TempVal2x4;
	blendDQ	=	TempVal2x4+blendDQ;
	TempVal2x4	=	world_dualquaternion_array_2x4[int(blendIndices.w)];
	SGX_AntipodalityAdjustment(initialDQ, TempVal2x4, TempVal2x4);
	TempVal2x4	=	blendWeights.w*TempVal2x4;
	blendDQ	=	TempVal2x4+blendDQ;
	SGX_NormalizeDualQuaternion(blendDQ);
	SGX_CalculateBlendPosition(BlendedPosition, blendDQ, TempVal4);
	FFP_Transform(viewproj_matrix, TempVal4, gl_Position);
	vec4 local_vertex = vertex;
	FFP_Transform(inverse_world_matrix, TempVal4, local_vertex);
	SGX_CalculateBlendNormal(normal, blendDQ, lLocalParam_1);
	vec3 local_normal = normal;
	FFP_Transform(inverse_world_matrix, lLocalParam_1, local_normal);
	lLocalParam_8	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_9	=	vec4(0.000000,0.000000,0.000000,0.000000);
	FFP_Transform(normal_matrix, local_normal, iTexcoord_0);
	FFP_Transform(worldview_matrix, local_vertex, iTexcoord_1);
}

