#version 400

uniform sampler2D u_borderFilter;

uniform vec2 u_size;
uniform vec2 u_position;
uniform float u_cornerSize;
uniform float u_borderSize;
uniform vec2 u_screenMax;
uniform vec2 u_screenMin;

in vec2 v_position;
in vec2 v_uv;

out vec4 outputColor;

void main(void)
{
	//check if within screen
	if( v_position.x > u_screenMax.x || v_position.x < u_screenMin.x
		|| v_position.y > u_screenMax.y || v_position.y < u_screenMin.y )
	{
		discard;
	}
	//check if in top left corner
	
	float distanceToEdge;

	vec2 RealBRCorner = u_position + u_size;

	vec2 TLCorner = vec2(min(u_position.x, RealBRCorner.x) + u_cornerSize, max(u_position.y, RealBRCorner.y) - u_cornerSize );
	vec2 BRConer = vec2(max(u_position.x, RealBRCorner.x) - u_cornerSize, min(u_position.y, RealBRCorner.y) + u_cornerSize);

	if( ! (v_position.x > TLCorner.x || v_position.y < TLCorner.y) )
	{ 
		vec2 TLA = vec2(TLCorner.x - u_cornerSize, TLCorner.y);
		vec2 TLB = vec2(TLCorner.x, TLCorner.y + u_cornerSize);
		float pointSignTL = dot(v_position - TLA, vec2(-1, 1) );
		if(  pointSignTL > 0 )
			discard;
		else
		{
			//get distance to corner segment to get distance
			vec2 linesegment = TLB - TLA;

			float t = dot( v_position - TLA, linesegment) / dot(linesegment, linesegment);

			t = clamp(t, 0, 1);

			vec2 closestPoint = TLA  + t * linesegment;
		
			distanceToEdge = distance(closestPoint, v_position) * 1.4;
		}
	}
	//check if in bottomr Right corner
	else if(  ! (v_position.x < BRConer.x || v_position.y > BRConer.y) )
	{
		//check bottom right corner
		vec2 BRA = vec2(BRConer.x + u_cornerSize, BRConer.y);
		vec2 BRB = vec2(BRConer.x, BRConer.y - u_cornerSize);
		float pointSignBL = dot(v_position - BRA, vec2(1, -1) );
		if(  pointSignBL > 0 )
			discard;
		else
		{
			//get distance to corner segment to get distance
			vec2 linesegment = BRB - BRA;

			float t = dot( v_position - BRA, linesegment) / dot(linesegment, linesegment);

			t = clamp(t, 0, 1);

			vec2 closestPoint = BRA  + t * linesegment;
		
			distanceToEdge = distance(closestPoint, v_position) * 1.4;
		}
	}
	else
	{
		float distanceY = min( abs(RealBRCorner.y - v_position.y), abs(u_position.y - v_position.y) );
		float distanceX = min( abs(RealBRCorner.x - v_position.x), abs(u_position.x - v_position.x) );


		distanceToEdge = min( distanceY, distanceX); 
	}

	
	if(distanceToEdge > u_borderSize)
		outputColor = vec4(.45, .45, .75, .6);
	else
		outputColor = texture2D(u_borderFilter, vec2(distanceToEdge / u_borderSize, 0.0) );

}