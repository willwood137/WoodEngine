#version 400

uniform mat4 u_mvp;
uniform sampler2D u_diffuse;

in vec2 a_uv;
in vec3 a_position;

out vec2 v_uv;


      vec4 GrayScale( vec4 Color )
      {
        float grayValue = Color.r * .2989 + Color.g * .5870 + Color.b * .1140;
        return vec4(grayValue, grayValue, grayValue, Color.a);
      }
    
void main(void)
{
v_uv = a_uv;
vec4 Transform_Vector_0_OutVector = (u_mvp)* vec4( (a_position), 1.0) ;
gl_Position = Transform_Vector_0_OutVector;

}