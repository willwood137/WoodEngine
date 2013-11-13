#version 400

uniform mat4 u_mvp;
uniform mat4 u_modelMatrix;
uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform mat4 u_viewMatrix;
uniform sampler2D u_diffuse;
uniform float u_time;

in vec2 v_uv;
in vec3 v_worldPosition;

layout(location = 0) out vec4 OutputColor;


void main(void)
{
vec4 TextureSampler_0_Color = texture2D( u_diffuse, v_uv) ;
float Constant_1_Constant = 0;
float Cosine_0_Out = cos( u_time) ;
float Constant_0_Constant = 2.4;
float Sum1_0_Sum = u_time+ Constant_0_Constant;
float Cosine_1_Out = cos( Sum1_0_Sum) ;
float Max_0_Max = max( Cosine_0_Out, Cosine_1_Out) ;
float Plus_One_div_2_0_newValue = ( Max_0_Max+ 1 ) * .5 ;
float Multiply1_1_Sum = Plus_One_div_2_0_newValue* Plus_One_div_2_0_newValue;
float Multiply1_0_Sum = Multiply1_1_Sum* Multiply1_1_Sum;
vec4 Append_Vector4_0_Vector4 = vec4( Constant_1_Constant, Multiply1_0_Sum, Multiply1_0_Sum, Constant_1_Constant) ;
vec4 Subtracion4_0_Difference = TextureSampler_0_Color- Append_Vector4_0_Vector4;
OutputColor = Subtracion4_0_Difference;

}