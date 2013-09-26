#version 120

uniform vec2 u_TopLeft;
uniform vec2 u_BottomRight;
uniform float u_middleLine;

uniform int u_debugInt;

attribute vec2 a_position;

varying vec2 v_position;

void main(void)
{
	v_position = a_position;
	gl_Position = vec4(a_position, .1, 1);
}