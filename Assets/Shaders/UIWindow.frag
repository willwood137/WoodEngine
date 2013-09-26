
uniform sampler2D u_diffuse;
uniform vec2 u_cursorPosition;

varying vec2 v_position;
varying vec2 v_texCoords;


void main()
{
	float illumination =  max(1, ( .15 - length(v_position - u_cursorPosition) ) * 10) ;
	gl_FragColor =  texture2D( u_diffuse, v_texCoords) * illumination;
}