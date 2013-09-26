
uniform sampler2D u_diffuse;

uniform vec4 u_color;
uniform float u_time;

varying vec2 v_texCoords;
varying vec3 v_worldPosition;
varying vec2 v_localPosition;


void main()
{
	gl_FragColor =  texture2D( u_diffuse, v_texCoords) * (u_color);
}