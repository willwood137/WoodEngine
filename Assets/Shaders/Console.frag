

uniform vec3 u_color;

uniform vec2 u_TopLeft;
uniform vec2 u_BottomRight;
uniform float u_middleLine;

varying vec2 v_position;

void main(void)
{

	float lineLength = .005;
	if(abs(v_position.x - u_TopLeft.x) < lineLength
		|| abs(v_position.x - u_BottomRight.x) < lineLength
		|| abs(v_position.y - u_TopLeft.y) < lineLength
		|| abs(v_position.y - u_BottomRight.y) < lineLength
		|| abs(v_position.y - u_middleLine) < lineLength )
		gl_FragColor = vec4(u_color, 1.0f);
	else
		gl_FragColor =  vec4(u_color * .1f, .65f);
}