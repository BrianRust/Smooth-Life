#version 330

//	fragment shader, GLSL 3.30
//
//	INPUTS
uniform vec3 u_cameraPosition;
uniform int u_wireframeBool;

in vec4 o_screenPosition;
in vec4 o_worldPosition;
in vec3 o_normal;

in vec4 o_surfaceColor;

//	OUTPUTS
out vec4 o_FragmentColor;

//------------------------------------------------------------------------------------------------------
void main()
{
	vec3 lightToPoint;
	vec3 totalDiffuseColorForAllLights = vec3(0.0, 0.0, 0.0);
	vec3 totalSpecularForAllLights = vec3(0.0, 0.0, 0.0);
	vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);

	lightToPoint = normalize(u_cameraPosition - o_worldPosition.xyz);

	totalDiffuseColorForAllLights += clamp(dot( o_normal, lightToPoint ), 0.0, 1.0) * lightColor.xyz * lightColor.w;

	o_FragmentColor.xyz = o_surfaceColor.xyz * totalDiffuseColorForAllLights * 0.7 * u_wireframeBool;
	o_FragmentColor.w = 1.0;
}