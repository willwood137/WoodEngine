#version 400

uniform mat4 u_mvp;
uniform sampler2D u_diffuse;

in vec2 v_uv;

out vec4 OutputColor;



      vec4 GrayScale( vec4 Color )
      {
        float grayValue = Color.r * .2989 + Color.g * .5870 + Color.b * .1140;
        return vec4(grayValue, grayValue, grayValue, Color.a);
      }
    
void main(void)
{
vec4 TextureSampler_0_Color = texture2D( (u_diffuse), (v_uv)) ;
vec4 GrayScale_0_GrayScale = GrayScale( (TextureSampler_0_Color)) ;
OutputColor = GrayScale_0_GrayScale;

}