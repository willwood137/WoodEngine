
uniform sampler2D u_diffuse;

uniform float u_isSelected;
varying vec2 v_texCoords;
varying vec3 v_worldPosition;
varying vec2 v_localPosition;


void main()
{
	gl_FragColor =  texture2D( u_diffuse, v_texCoords) * u_isSelected;
}