#version 400

uniform sampler2D u_backgroundTexture;

uniform vec2 u_size;
uniform vec2 u_position;
uniform float u_cornerSize;
uniform float u_borderSize;
uniform vec2 u_mousePos;

in vec2 v_position;
in vec2 v_canvasPosition;
in vec2 v_uv;

out vec4 outputColor;

void main(void)
{
	float mouseDistance = distance( v_canvasPosition, u_mousePos);
	float value;
	if(mouseDistance < 20)
		value = smoothstep(0, 1,  min(pow(mouseDistance, .5) * .25, 1) );
	else
		value = 1.0;
	outputColor = ( (1 - value) * vec4(0.0, 1.0, 1.0, 1.0) ) + ( (value) * texture2D(u_backgroundTexture, v_uv ) );
	//outputColor = (1.0, 1.0, 1.0, 1.0);
// 	if(mouseDistance > 20)
// 		outputColor = texture2D(u_backgroundTexture, v_uv );
// 	else
// 		outputColor = vec4(0.0, 1.0, 1.0, 1.0);

}