#version 120

uniform vec2 u_position;
uniform float u_size;

attribute vec2 a_position;

void main(void)
{
	gl_Position = vec4( a_position*u_size + u_position, .022, 1);
}