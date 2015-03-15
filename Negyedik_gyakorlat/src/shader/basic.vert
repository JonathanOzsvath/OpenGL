#version 430
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Light;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform vec4 LightPosition;

void main(){
	vec3 tnorm = normalize( NormalMatrix * VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition,1.0);
    vec3 s = normalize(vec3(LightPosition - eyeCoords));

	Light = vec3( 1.0f, 1.0f, 1.0f) * vec3(0.9f, 0.5f, 0.3f) * max(dot(s, tnorm), 0.0);

	gl_Position = MVP * vec4(VertexPosition, 1.0);
}
 