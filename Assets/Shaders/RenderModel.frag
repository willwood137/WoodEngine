uniform sampler2D u_diffuse;
uniform sampler2D u_normalMap;
uniform sampler2D u_emissiveMap;
uniform sampler2D u_specularMap;

uniform int u_hasNormalMap;
uniform int u_hasEmissiveMap;
uniform int u_hasSpecularMap;

uniform float u_time;
uniform vec3 u_light1;

varying vec3 v_pos;
varying vec3 v_normal;
varying vec4 v_screenPos;
varying vec3 v_lightDirTangentSpace;
varying vec3 v_viewDirTangentSpace;

void main(void)
{
	float LightIntensity = 0;
 
 	if( u_hasNormalMap != 0 )
 	{
 		vec3 normalFragment = texture2D(u_normalMap, gl_TexCoord[0].st).xyz;
 		normalFragment *= 2;
 		normalFragment -= vec3(1.0, 1.0, 1.0);
 
 		normalFragment = normalize(normalFragment);
 		
 		vec3 lightDir = normalize(v_lightDirTangentSpace);
 		LightIntensity = max(0, dot(normalFragment, lightDir ) );
 
 	}
 	else
 	{
		LightIntensity = dot(v_normal, normalize(u_light1 - v_pos) ); 
 	}
	
	LightIntensity = max(.05, LightIntensity);

	
	gl_FragColor = texture2D( u_diffuse, gl_TexCoord[0].st) * vec4(LightIntensity, LightIntensity, LightIntensity, 1);
	
}