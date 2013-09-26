#version 400

uniform mat4 u_mvp;

in vec2 v_uv;

out vec4 OutputColor;

void main(void)
{
vec4 TextureSampler_1_Color = sampler2D(0, v_uv ) ;
vec4 TextureSampler_2_Color = sampler2D(0, v_uv ) ;
vec4 Sum_1_Sum = TextureSampler_1_Color + TextureSampler_2_Color ;
OutputColor = Sum_1_Sum;

}