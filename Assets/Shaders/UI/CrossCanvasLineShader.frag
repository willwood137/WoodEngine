#version 400

uniform vec3	u_lineSegmentsStrip[10];
uniform float	u_lineLengths[10];
uniform float	u_fullLength;
uniform int		u_numLines;
uniform float	u_startSize;
uniform float	u_endSize; 
uniform vec2	u_size;

uniform sampler2D	u_filter;

uniform vec2 u_screenMax;
uniform vec2 u_screenMin;

in vec2 v_position;
in vec2 v_canvasPosition;

out vec4 outputColor;

void main(void)
{


 
 	//check position against each line
 	float shortestDistance = 9000.0;
	float totalLength = 0.0f;
 
 	for( int i = 0; i < u_numLines; ++i)
 	{
 		// Consider the line extending the segment, parameterized as v + t (w - v).
 		 // We find projection of point p onto the line. 
 		// It falls where t = [(p-v) . (w-v)] / |w-v|^2
 		vec2 linesegment = u_lineSegmentsStrip[i+1] - u_lineSegmentsStrip[i];
 
		
 		float t = dot( v_canvasPosition - u_lineSegmentsStrip[i], linesegment) / dot(linesegment, linesegment);
		
		
 		t = clamp(t, 0, 1);
 

 		vec2 closestPoint = u_lineSegmentsStrip[i]  + t * linesegment;
 		
 		float newdistance = distance(closestPoint, v_canvasPosition); 
 
 		if(newdistance < shortestDistance)
		{
 			shortestDistance = newdistance;
			
			totalLength = t * u_lineLengths[i];
			for(int n = 0; n < i; n++)
			{
				totalLength += u_lineLengths[n];
			}
				 
		}
 	}
 
 	if(shortestDistance < smoothstep( u_startSize, u_endSize, totalLength / u_fullLength ) )
 		outputColor = texture2D(u_filter, vec2( shortestDistance / u_lineSize), 0.0 );
 	else
		discard;
}