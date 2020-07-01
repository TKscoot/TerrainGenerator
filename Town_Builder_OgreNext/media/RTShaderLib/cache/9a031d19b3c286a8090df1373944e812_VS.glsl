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

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	mat4	worldviewproj_matrix;
uniform	mat3	normal_matrix;
uniform	vec4	derived_ambient_light_colour;
uniform	vec4	surface_emissive_colour;
uniform	vec4	derived_scene_colour;
uniform	float	surface_shininess;
uniform	vec4	light_position_view_space;
uniform	vec4	light_attenuation;
uniform	vec4	derived_light_diffuse_colour;
uniform	vec4	light_position_view_space1;
uniform	vec4	derived_light_diffuse_colour1;
uniform	mat4	worldview_matrix;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec4	vertex;
in	vec3	normal;
out	vec4	iColor_0;
out	vec3	iTexcoord_0;
out	vec4	iTexcoord_1;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;
	vec3	lLocalParam_2;
	vec3	lLocalParam_3;
	vec3	lLocalParam_4;
	vec3	lLocalParam_5;

	FFP_Transform(worldviewproj_matrix, vertex, gl_Position);
	lLocalParam_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	iColor_0	=	lLocalParam_0;
	lLocalParam_1	=	vec4(0.000000,0.000000,0.000000,0.000000);
	iTexcoord_0	=	normal;
	iTexcoord_1	=	vertex;
	FFP_Transform(normal_matrix, normal, lLocalParam_2);
	FFP_Transform(worldview_matrix, vertex, lLocalParam_3);
	iColor_0	=	derived_scene_colour;
	lLocalParam_5	=	vec3(-1.00000,-1.00000,-1.00000)*lLocalParam_3;
	lLocalParam_4	=	light_position_view_space.xyz-lLocalParam_3;
	SGX_Light_Point_Diffuse(lLocalParam_2, lLocalParam_4, light_attenuation, derived_light_diffuse_colour.xyz, iColor_0.xyz);
	SGX_Light_Directional_Diffuse(lLocalParam_2, light_position_view_space1.xyz, derived_light_diffuse_colour1.xyz, iColor_0.xyz);
}

