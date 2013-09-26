#version 120


uniform vec2 u_position;
uniform vec2 u_offset;
uniform vec4 u_color;
uniform float u_size;

uniform vec2 u_inverseScreenResolution;

attribute vec2 a_position;
attribute vec2 a_texCoords;

varying vec2 v_texCoords;

void main(void)
{
	v_texCoords = a_texCoords;
	
	vec2 screenPosition = ( (u_position + a_position * u_size + u_offset) * u_inverseScreenResolution ) * 2 - vec2(1, 1);

	gl_Position = vec4( screenPosition, .023, 1);
}