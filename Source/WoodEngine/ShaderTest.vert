#version 400

uniform mat4 u_mvp;

in vec3 a_uv;
in vec3 a_position;
in vec3 a_normal;

out vec2 v_uv;
void main(void)
{
vec4 Transform_Vector_0_OutVector = u_mvp * vec4( a_position , 1.0) ;
gl_Position = Transform_Vector_0_OutVector;
v_uv = a_normal;

}