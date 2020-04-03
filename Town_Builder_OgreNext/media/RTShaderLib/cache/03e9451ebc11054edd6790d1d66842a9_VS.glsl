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
#include "FFPLib_Texturing.glsl"
#include "SGXLib_NormalMap.glsl"
#include "FFPLib_Common.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat4	worldviewproj_matrix;
uniform	vec3	camera_position;
uniform	mat4	world_matrix;
uniform	mat3	inv_world_rotation_matrix0;
uniform	vec4	light_position;
uniform	vec4	light_direction_obj_space0;
uniform	mat3	inv_world_rotation_matrix1;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec3	normal;
in	vec3	tangent;
in	vec4	uv0;
out	vec2	iTexcoord_0;
out	vec3	iTexcoord_1;
out	vec3	iTexcoord_2;
out	vec3	iTexcoord_3;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;
	mat3	lMatTBN;
	vec3	lNormalMapTempDir;
	vec3	lLocalParam_4;

	FFP_Transform(worldviewproj_matrix, vertex, gl_Position);
	lLocalParam_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_1	=	vec4(0.000000,0.000000,0.000000,0.000000);
	SGX_ConstructTBNMatrix(normal, tangent, lMatTBN);
	iTexcoord_0	=	uv0.xy;
	FFP_Transform(world_matrix, vertex, lLocalParam_4);
	lNormalMapTempDir	=	camera_position.xyz-lLocalParam_4.xyz;
	FFP_Transform(inv_world_rotation_matrix1, lNormalMapTempDir, lNormalMapTempDir);
	FFP_Transform(lMatTBN, lNormalMapTempDir, iTexcoord_1);
	lNormalMapTempDir	=	light_position.xyz-lLocalParam_4;
	FFP_Transform(inv_world_rotation_matrix1, lNormalMapTempDir, lNormalMapTempDir);
	FFP_Transform(lMatTBN, lNormalMapTempDir, iTexcoord_2);
	FFP_Transform(lMatTBN, light_direction_obj_space0.xyz, iTexcoord_3);
	iTexcoord_0	=	uv0.xy;
}

