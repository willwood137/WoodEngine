uniform int u_debugInt;
uniform vec4 u_color;

void main(void)
{
	if( u_debugInt == 1)
		gl_FragColor =  vec4(u_color);
	else
		discard;
}