#version 120

uniform mat4 u_MV;
uniform vec2 u_position;
uniform vec2 u_size;
uniform float u_height;
uniform int u_rotate;

attribute vec3 a_position;
attribute vec2 a_texCoords;

varying vec2 v_texCoords;
varying vec3 v_worldPosition;
varying vec2 v_localPosition;

void main()
{	
	v_texCoords = a_texCoords;
	
	v_localPosition = vec2(a_position.x, a_position.z);
	
	if( u_rotate == 90)
	{
		v_localPosition = vec2(-a_position.z + .5, a_position.x);
	}
	else if( u_rotate == 180)
	{
		v_localPosition = vec2(-a_position.x, -a_position.z) + .5;
	}
	else if( u_rotate == 270)
	{
		v_localPosition = vec2(a_position.z, -a_position.x + .5);
	}	

	v_localPosition *= u_size;

	v_worldPosition = vec3( u_position.x, u_height * u_size.x, u_position.y) + vec3(v_localPosition.x, a_position.y * u_size.x, v_localPosition.y);
	gl_Position = gl_ModelViewProjectionMatrix * u_MV * vec4(v_worldPosition, 1);
}