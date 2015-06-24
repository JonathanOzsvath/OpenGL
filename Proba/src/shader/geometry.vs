#version 430

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

uniform mat4 MVP;

out VS_OUT {
    vec3 color;
} vs_out;

void main()
{
    gl_Position = MVP * vec4(position.x, position.y, 0.0f, 1.0f); 
    vs_out.color = color;
}
