
attribute vec3 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_TextureCoordinates;

uniform mat4 u_ModelTransform;

varying vec3 v_Normal;
varying vec2 v_TextureCoordinates;

void main() 
{
	gl_Position = gl_ModelViewProjectionMatrix * u_ModelTransform * vec4(a_Position, 1.0);
	v_Normal = (u_ModelTransform * vec4(a_Normal, 1.0) ).xyz;
	v_TextureCoordinates = a_TextureCoordinates;
}