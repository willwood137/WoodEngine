#version 400

uniform mat4 u_mvp;
uniform mat4 u_modelMatrix;
uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform mat4 u_viewMatrix;
uniform sampler2D u_diffuse;
uniform vec3 u_light;

in vec2 a_uv;
in vec3 a_normal;
in vec3 a_position;

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_worldPosition;


      vec4 Phong_Lighting( sampler2D diffuse, vec2 textureCoordinates, vec3 normal, vec3 worldPosition, vec3 lightPosition)
      {
      float LightIntensity = dot( normal, normalize(lightPosition - worldPosition) );
      vec4 difResult = texture2D( diffuse, textureCoordinates );
      return  vec4( difResult.rgb * LightIntensity, difResult.a);
      }

    
void main(void)
{
v_normal = a_normal;
v_uv = a_uv;
vec4 Transform_Vector_1_OutVector = u_mvp* vec4(	a_position, 1.0 );
gl_Position = Transform_Vector_1_OutVector;
vec4 Transform_Vector_0_OutVector = u_modelMatrix* vec4(	a_position, 1.0 );
v_worldPosition = Transform_Vector_0_OutVector.xyz;

}