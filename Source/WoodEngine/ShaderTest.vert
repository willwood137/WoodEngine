#version 400

uniform mat4 u_mvp;
uniform mat4 u_modelMatrix;
uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform mat4 u_viewMatrix;
uniform sampler2D u_diffuse;
uniform float u_time;
uniform vec3 u_light;

in vec2 a_uv;
in vec3 a_normal;
in vec3 a_position;

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_worldPosition;


      vec4 Twist( vec4 Position, float twistAmount )
      {

      float ySquared = twistAmount * Position.y  * Position.y * .00314159;
      float cosA = cos(ySquared);
      float sinA = sin(ySquared);

      float newX = Position.x * cosA + ( Position.z * -sinA );
      float newZ = Position.x * sinA + ( Position.z * cosA );

      return vec4(newX, Position.y, newZ, 1.0);
      }
    

      vec4 Phong_Lighting( sampler2D diffuse, vec2 textureCoordinates, vec3 normal, vec3 worldPosition, vec3 lightPosition)
      {
      float LightIntensity = dot( normal, normalize(lightPosition - worldPosition) );
      vec4 difResult = texture2D( diffuse, textureCoordinates );
      return  vec4( difResult.rgb * LightIntensity, difResult.a);
      }

    
void main(void)
{
v_uv = a_uv;
vec4 Transform_Vector_0_OutVector = u_modelMatrix* vec4(	a_position, 1.0 );
float Cosine_0_Out = cos( u_time) ;
vec4 Twist_0_Twisted_Position = Twist( Transform_Vector_0_OutVector, Cosine_0_Out) ;
vec4 Transform_Vector_1_OutVector = u_mvp* Twist_0_Twisted_Position;
gl_Position = Transform_Vector_1_OutVector;
v_worldPosition = Twist_0_Twisted_Position.xyz;
v_normal = a_normal;

}