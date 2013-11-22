#version 400

uniform mat4 u_mvp;
uniform mat4 u_modelMatrix;
uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform mat4 u_viewMatrix;
uniform sampler2D u_diffuse;

in vec2 a_uv;
in vec3 a_normal;
in vec3 a_position;

out vec2 v_uv;
out vec3 v_worldPosition;

void main(void)
{
v_uv = a_uv;
vec4 Transform_Vector_1_OutVector = u_mvp* vec4( a_position, 1.0 );
gl_Position = Transform_Vector_1_OutVector;
vec4 Constant4_0_Vec4 = vec4( 1, 1, 1, 1) ;
v_worldPosition = Constant4_0_Vec4.xyz;

}