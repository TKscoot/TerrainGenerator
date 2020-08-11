#version 120

attribute vec4 position;
attribute vec2 uv0;

uniform mat4 world;
uniform mat4 worldViewProj;

varying vec2 oUV0;
varying vec4 pos;

void main()
{
	gl_Position = worldViewProj * position;
	oUV0 = uv0;
	pos = vec4(world * vec4(position.xyz, 1.0));

}