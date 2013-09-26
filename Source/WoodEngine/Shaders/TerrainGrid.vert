#version 130


uniform mat4 u_MV;

uniform vec4 u_color;

uniform vec2 u_minGrid;
uniform vec2 u_maxGrid;

uniform float u_time;
uniform float u_debugFloat;
uniform int u_debugInt;

attribute vec2 a_position;

varying vec3 v_position;

void main() 
{
	v_position = vec3(a_position.x, -1., a_position.y);

	gl_Position = gl_ModelViewProjectionMatrix * u_MV * vec4(v_position, 1);	
}