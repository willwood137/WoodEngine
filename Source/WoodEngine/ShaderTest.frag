#version 400

uniform mat4 u_mvp;
uniform sampler2D u_diffuse;

in vec2 v_uv;

out vec4 OutputColor;

void main(void)
{
vec4 TextureSampler_0_Color = texture2D( (u_diffuse), (v_uv)) ;
OutputColor = TextureSampler_0_Color;

}