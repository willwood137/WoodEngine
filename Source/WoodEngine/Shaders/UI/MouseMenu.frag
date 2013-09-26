#version 400

uniform vec2	u_position;
uniform int		u_selectedBox;
uniform float	u_boxHeight;
uniform float	u_lineWidth;
uniform vec2	u_size;

uniform vec4	u_backgroundColor;
uniform vec4	u_lineColor;


in vec2 v_position;

out vec4 outputColor;

void main(void)
{
	//check x
	float xValue = v_position.x - u_position.x;

	if(xValue <= u_lineWidth || xValue >= ( u_size.x - u_lineWidth) )
	{
		outputColor = u_lineColor;
	} 
	else
	{
		float yValue = v_position.y - u_position.y;

		float remainder = mod( yValue, u_boxHeight);
		
		if(remainder <= u_lineWidth * .5 || remainder >= (u_boxHeight - u_lineWidth * .5) )
		{	
			outputColor = u_lineColor;
		}
		else
		{
			outputColor = u_backgroundColor;
		}
	}
}