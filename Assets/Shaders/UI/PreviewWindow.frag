#version 400

uniform sampler2D u_diffuse;

uniform vec2 u_size;
uniform vec2 u_position;
uniform vec4 u_color;
uniform vec2 u_screenMax;
uniform vec2 u_screenMin;

in vec2 v_position;
in vec2 v_uv;

out vec4 outputColor;

void main(void)
{
	//check if within screen
	if( v_position.x > u_screenMax.x || v_position.x < u_screenMin.x
		|| v_position.y > u_screenMax.y || v_position.y < u_screenMin.y )
	{
		discard;
	}


	outputColor = texture2D(u_diffuse, v_uv);
}