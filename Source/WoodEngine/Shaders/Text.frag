
uniform sampler2D u_diffuse;
uniform vec4 u_color;

varying vec2 v_texCoords;

void main(void)
{
	gl_FragColor =  texture2D( u_diffuse, v_texCoords) * u_color;
}