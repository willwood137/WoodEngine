#version 120

uniform vec2 u_windowPosition;
uniform vec3 u_position;
uniform vec3 u_size;


attribute vec2 a_position;
attribute vec2 a_texCoords;

varying vec2 v_position;
varying vec2 v_texCoords;

void main() 
{
	v_position = u_windowPosition + u_position.xy + a_position*u_size.xy;
	if(u_size.z == 1)
		v_texCoords = vec2(u_size.x * a_texCoords.y, u_size.y * a_texCoords.x);
	else if( u_size.z == 0)
	{
		v_texCoords = u_size.xy * a_texCoords;
	}
	else if( u_size.z == 2)
	{
		v_texCoords = a_texCoords;
	}
		
	gl_Position = vec4(v_position, u_position.z, 1);
}