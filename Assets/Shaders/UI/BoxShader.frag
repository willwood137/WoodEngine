#version 400

uniform sampler2D u_diffuse;

uniform vec2 u_size;
uniform vec2 u_position;
uniform vec4 u_color;
uniform vec4 u_borderColor;
uniform float u_borderSize;
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

	float distanceToEdge = min( min( abs( v_position.x - u_position.x ), abs( v_position.x - (u_position.x + u_size.x) ) ),
							min( abs( v_position.y - u_position.y ), abs( v_position.y - (u_position.y + u_size.y) ) ) );

	if(distanceToEdge < u_borderSize )
		outputColor = u_borderColor;
	else
		outputColor = u_color;
}