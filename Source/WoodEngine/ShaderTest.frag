#version 400

uniform mat4 u_mvp;
uniform mat4 u_modelMatrix;
uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform mat4 u_viewMatrix;
uniform sampler2D u_diffuse;

in vec2 v_uv;
in vec3 v_worldPosition;

layout(location = 0) out vec4 OutputColor;


void main(void)
{
vec4 TextureSampler_0_Color = texture2D( u_diffuse, v_uv) ;
OutputColor = TextureSampler_0_Color;

}