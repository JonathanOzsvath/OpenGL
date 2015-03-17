#version 430
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 color;

//model és a view matrix szorzata
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
//model
uniform mat4 MVP;
uniform vec4 LightPosition;

uniform vec3 Ld;
uniform vec3 Kd;

void main()
{
	vec3 tnorm = normalize( NormalMatrix * VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition,1.0);
    vec3 s = normalize(vec3(LightPosition - eyeCoords));

	color = Ld * Kd * max(dot(s, tnorm), 0.0);

	gl_Position = MVP * vec4(VertexPosition,1.0);
}