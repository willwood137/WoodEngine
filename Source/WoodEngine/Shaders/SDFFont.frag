#version 120

uniform sampler2D u_diffuse;
uniform vec4 u_color;

varying vec2 v_texCoords;

void main(void)
{
	// retrieve distance from texture
	float mask = texture2D( u_diffuse, v_texCoords).a;

	// use current drawing color
	vec4 clr;
	clr.rgb = u_color.rgb;
 
	// perform simple thresholding
// 	if( mask < 0.1 )  
// 		clr.a = 0.0;
// 	else if( mask < 0.5 )
// 		clr = vec4(0.0, 0.0, 0.0, 1.0);
// 	else
// 		clr.a = 1.0;

	if( mask >= .02)
		clr.a = 1.0;
	else
		clr.a = 0.0f;

	// do some anti-aliasing
	clr.a *= smoothstep(0.0, 1.0, mask * 1.2);

	// final color
	gl_FragColor = u_color * clr.a; 
}