#version 430
layout (location = 0) in vec3 VertexPosition;
out vec3 color;
void main()
{
	color = vec3(1.0,0.0,0.0);
	gl_Position = vec4(VertexPosition,1.0);
}