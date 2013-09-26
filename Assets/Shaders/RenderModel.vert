uniform mat4 u_worldTransform[100];
uniform mat4 u_viewProjectionMatrix;
uniform int u_instanced;

uniform float u_time;
uniform vec3 u_light1;

uniform int u_hasNormalMap;
uniform int u_hasEmissiveMap;
uniform int u_hasSpecularMap;

attribute vec3 a_position;
attribute vec3 a_tangent;
attribute vec3 a_biTangent;
attribute float a_instanceID;

varying vec3 v_pos;
varying vec3 v_normal;
varying vec4 v_screenPos;
varying vec3 v_lightDirTangentSpace;
varying vec3 v_viewDirTangentSpace;


void main() 
{
	int instanceID = u_instanced * int(a_instanceID);
	vec4 vWorld = (u_worldTransform[instanceID] * gl_Vertex );
	gl_Position = gl_ModelViewProjectionMatrix * u_viewProjectionMatrix * vWorld;
	v_pos = vWorld.xyz;
	gl_TexCoord[0] = gl_MultiTexCoord0;

	if( u_hasNormalMap != 0)
	{
		
		//vec3 view = (u_camPos - vWorld.xyz);
		vec3 lightDir = (u_light1 - vWorld.xyz);
		vec3 tangentWorld = ( u_worldTransform[instanceID] *  vec4(a_tangent, 0)).xyz;
		vec3 biTangentWorld = ( u_worldTransform[instanceID] *  vec4(a_biTangent, 0)).xyz;
		vec3 normalWorld = ( u_worldTransform[instanceID] *  vec4(gl_Normal, 0)).xyz;

		//v_viewDirTangentSpace = (vec3(dot(tangentWorld, view), dot(biTangentWorld, view), dot( normalWorld, view) ) );
		v_lightDirTangentSpace = (vec3(dot(tangentWorld, lightDir), dot(biTangentWorld, lightDir), dot( normalWorld, lightDir) ) );
	}
	else
	{
		v_normal = normalize( ( u_worldTransform[instanceID] * vec4(gl_Normal, 0)).xyz);
	}
}