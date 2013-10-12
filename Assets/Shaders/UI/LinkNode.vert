#version 400

uniform vec2 u_position;
uniform vec2 u_size;
uniform vec2 u_inverseScreenResolution;
uniform vec2 u_screenMax;
uniform vec2 u_screenMin;
uniform float u_layer;


in vec2 in_position;

out vec2 v_position;
out vec2 v_uv;

void main(void)
{
	v_uv = in_position;
	v_position = (u_position + in_position * u_size);
	gl_Position = vec4( v_position * u_inverseScreenResolution * 2 - vec2(1, 1), u_layer, 1.0);
}