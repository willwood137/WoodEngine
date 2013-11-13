#version 400

uniform vec2 u_size;
uniform vec2 u_position;
uniform vec4 u_color;
uniform vec2 u_screenMax;
uniform vec2 u_screenMin;

in vec2 v_position;

out vec4 outputColor;

void main(void)
{
	//check if within screen
	if( v_position.x > u_screenMax.x || v_position.x < u_screenMin.x
		|| v_position.y > u_screenMax.y || v_position.y < u_screenMin.y )
	{
		discard;
	}


	vec2 TRCorner = u_size + u_position;
	float distanceY = min( abs(TRCorner.y - v_position.y), abs(u_position.y - v_position.y) );
	float distanceX = min( abs(TRCorner.x - v_position.x), abs(u_position.x - v_position.x) );


	float distanceToEdge = min( distanceY, distanceX); 

	if(distanceToEdge > 3.0)
		outputColor = u_color;
	else
		outputColor = vec4(0.0, 0.0, 0.0, 1.0);
}