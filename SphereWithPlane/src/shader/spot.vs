#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 Normal;
out vec3 Position;
out vec2 TexCoord;
out vec4 ProjTexCoord;
out vec4 ShadowCoord;

uniform mat4 Model;
uniform mat4 ModelView;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ProjectorMatrix;
uniform mat4 ShadowMatrix;

void main()
{
	vec4 pos4 = vec4(VertexPosition,1.0);

    Normal = normalize( NormalMatrix * VertexNormal);
    Position = vec3( ModelView * pos4);
	TexCoord = VertexTexCoord;
	ProjTexCoord = ProjectorMatrix * (Model * pos4);
	ShadowCoord = ShadowMatrix * vec4(VertexPosition, 1.0f);

    gl_Position = MVP * pos4;
}
