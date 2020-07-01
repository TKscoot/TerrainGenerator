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
#include "SGXLib_LayeredBlending.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

uniform	sampler2D	gTextureSampler0;
uniform	sampler2D	gTextureSampler1;
uniform	vec4	custom2;

//-----------------------------------------------------------------------------
// Function Name: main
// Function Desc: 
//-----------------------------------------------------------------------------
in	vec2	iTexcoord_0;
in	vec2	iTexcoord_1;
out vec4	oColor_0;
void main(void) {
	vec4	lLocalParam_0;
	vec4	lLocalParam_1;
	vec4	texel_0;
	vec4	texel_1;

	lLocalParam_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lLocalParam_1	=	vec4(0.000000,0.000000,0.000000,0.000000);
	oColor_0	=	lLocalParam_0;
	texel_0	=	texture2D(gTextureSampler0, iTexcoord_0);
	texel_1	=	texture2D(gTextureSampler1, iTexcoord_1);
	oColor_0	=	texel_0*oColor_0;
	SGX_src_mod_inv_modulate(texel_1, custom2, texel_1);
	SGX_blend_luminosity(texel_1, oColor_0, oColor_0);
	oColor_0.xyz	=	oColor_0.xyz+lLocalParam_1.xyz;
}

