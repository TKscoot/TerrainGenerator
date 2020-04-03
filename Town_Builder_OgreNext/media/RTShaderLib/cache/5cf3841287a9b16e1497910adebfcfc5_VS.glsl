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
#include "SGXLib_PerPixelLighting.glsl"
#include "FFPLib_Common.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat4	worldviewproj_matrix;
uniform	mat3	normal_matrix;
uniform	mat4	worldview_matrix;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	mat3x4	uv1;
in	vec3	normal;
in	vec4	uv0;
out	vec3	iTexcoord_0;
out	vec3	iTexcoord_1;
out	vec2	iTexcoord_2;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;

	vec4 local_vertex = vertex;
	FFP_Transform(uv1, local_vertex, local_vertex.xyz);
	vec3 local_normal = normal;
	FFP_Transform(uv1, local_normal, local_normal);
	FFP_Transform(worldviewproj_matrix, local_vertex, gl_Position);
	lLocalParam_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_1	=	vec4(0.000000,0.000000,0.000000,0.000000);
	FFP_Transform(normal_matrix, local_normal, iTexcoord_0);
	FFP_Transform(worldview_matrix, local_vertex, iTexcoord_1);
	iTexcoord_2	=	uv0.xy;
}

