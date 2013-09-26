
uniform vec2 u_size;
uniform vec3 u_position;
uniform mat4 u_MV;

attribute vec2 a_texCoords;

varying vec2 v_texCoords;

void main() {

	

	vec2 offset = (a_texCoords + vec2(-.5, -.5));

	offset.y *= -1;
	
	vec3 axis1 = vec3(	u_viewMatrix[0][0], 
						u_viewMatrix[1][0],
						u_viewMatrix[2][0]);

	vec3 axis2 = vec3(	u_viewMatrix[0][1], 
						u_viewMatrix[1][1],
						u_viewMatrix[2][1]);

	vec4 pos = vec4( (offset.x*axis1*u_size.x + offset.y*axis2*u_size.y) + u_position, 1); 
	gl_Position = gl_ModelViewProjectionMatrix * u_viewMatrix * pos;
	v_texCoords = a_textureCoords;
}
			
	