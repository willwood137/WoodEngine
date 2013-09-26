#version 400

in vec2 in_position;

out vec2 v_positoin;
//uniform vec4[3] input_LineSegments;

void main(void)
{
	v_position = in_position;
	gl_Position = vec4( in_position, .023, 1 );
}