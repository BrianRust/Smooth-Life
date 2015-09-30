#version 330

//	vertex shader, GLSL 3.30
//
//	INPUTS
//		gl_Vertex : vertex position in object space (raw, from VBO)
//		gl_ModelViewProjectionMatrix : concatenation of GL_MODELVIEW and GL_PROJECTION
//			typically the world-to-screen transform
uniform mat4 u_modelViewProjectionMatrix;

in vec4 i_Vertex;
in float i_Normal;
in float i_Color;

//	OUTPUTS
//		gl_Position : vertex position in screen space (for OpenGL to make triangles out of)
//		o_screenPosition : vertex position in screen space (for me, to pass to my own fragment shader)

out vec4 o_screenPosition;
out vec4 o_worldPosition;
out vec3 o_normal;
out vec4 o_surfaceColor;

void main()
{
	vec4 floatVertex = i_Vertex;
	
	gl_Position = u_modelViewProjectionMatrix * floatVertex;
	o_screenPosition = gl_Position;
	o_worldPosition = floatVertex;

	o_normal = vec3(0.0, 0.0, 0.0);
	o_surfaceColor = vec4(i_Color, i_Color, i_Color, 1.0);

	//0
	o_normal += vec3(0.0, -1.0, 0.0);
	
	//1
	o_normal += vec3(1.0, 1.0, 0.0) * mix(1.0, 0.0, clamp(sign(1.0 - i_Normal), 0.0, 1.0));

	//2
	o_normal += vec3(-1.0, 1.0, 0.0) * mix(1.0, 0.0, clamp(sign(2.0 - i_Normal), 0.0, 1.0));

	//3
	o_normal += vec3(0.0, -1.0, 1.0) * mix(1.0, 0.0, clamp(sign(3.0 - i_Normal), 0.0, 1.0));

	//4
	o_normal += vec3(-1.0, 0.0, -1.0) * mix(1.0, 0.0, clamp(sign(4.0 - i_Normal), 0.0, 1.0));

	//5
	o_normal += vec3(1.0, 0.0, -1.0) * mix(1.0, 0.0, clamp(sign(5.0 - i_Normal), 0.0, 1.0));
}