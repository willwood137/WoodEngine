uniform vec4 u_color;

uniform vec2 u_minGrid;
uniform vec2 u_maxGrid;

uniform float u_time;
uniform float u_debugFloat;
uniform int u_debugInt;


varying vec3 v_position;

void main()
{
	float distance = 5;
	float temp;
	if( v_position.z < u_minGrid.x)
	{
		temp = u_minGrid.x - v_position.z;
		distance += temp * temp;
	}	
	if( v_position.x < u_minGrid.y)
	{
		temp = u_minGrid.y - v_position.x;
		distance += temp * temp;
	}
				
	if( v_position.z > u_maxGrid.x)
	{
		temp = v_position.z - u_maxGrid.x;
		distance += temp * temp;
	}

	if( v_position.x > u_maxGrid.y)
	{
		temp = v_position.x - u_maxGrid.y;
		distance += temp * temp;
	}

	float alpha = 50 / distance;

	gl_FragColor = vec4(u_color.xyz, u_color.w * alpha);
}