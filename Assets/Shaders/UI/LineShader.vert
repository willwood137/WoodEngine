#version 400

uniform vec2 u_position;
uniform vec2 u_size;
uniform vec2 u_canvasMin;
uniform vec2 u_canvasMax;
uniform vec2 u_inverseScreenResolution;

in vec2 in_position;

out vec2 v_position;
out vec2 v_canvasPosition;

void main(void)
{
	v_canvasPosition = (u_canvasMin + in_position * (u_canvasMax - u_canvasMin) );
	v_position = (u_position + in_position * u_size);
	gl_Position = vec4( v_position * u_inverseScreenResolution * 2 - vec2(1, 1), 0.0, 1.0);
}