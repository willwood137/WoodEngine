#version 400

uniform mat4 u_mvp;
uniform mat4 u_modelMatrix;
uniform mat4 u_modelView;
uniform mat4 u_projection;
uniform mat4 u_viewMatrix;
uniform sampler2D u_diffuse;
uniform float u_time;
uniform vec3 u_light;

in vec2 v_uv;
in vec3 v_normal;
in vec3 v_worldPosition;

layout(location = 0) out vec4 OutputColor;



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
vec4 Phong_Lighting_0_Color = Phong_Lighting( u_diffuse, v_uv, v_normal, v_worldPosition, u_light) ;
OutputColor = Phong_Lighting_0_Color;

}