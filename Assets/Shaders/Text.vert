#version 120

uniform mat4 u_MV;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_worldTransform;

uniform vec3 u_position;
uniform float u_offset;
uniform vec4 u_color;
uniform float u_size;

uniform int u_debugInt;

attribute vec2 a_position;
attribute vec2 a_texCoords;

varying vec2 v_texCoords;

void main(void)
{
	v_texCoords = a_texCoords;

	if(u_debugInt == 2)
	{
		gl_Position = vec4( u_position.xy + a_position * u_size + vec2(u_offset, 0), .023, 1);
	}
	else if(u_debugInt == 1)
	{
		vec4 vWorld = vec4(u_position, 1.0f) + u_worldTransform * vec4( 0.0f, a_position.y * u_size, (a_position.x * u_size + u_offset), 1.0f);
		gl_Position = gl_ModelViewProjectionMatrix * u_MV * vWorld;
	}
	else if(u_debugInt == 3)
	{
		vec4 vWorld = vec4(u_position, 1.0f) + u_worldTransform * vec4( 0.0f, a_position.y * u_size, (a_position.x * u_size + u_offset), 1.0f);
		gl_Position = gl_ModelViewProjectionMatrix * u_MV * vWorld;
	}
}