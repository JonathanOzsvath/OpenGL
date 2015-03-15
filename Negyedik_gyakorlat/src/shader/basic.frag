#version 430
in vec3 Light;
layout(location = 0) out vec4 fragColor;
void main() {
fragColor = vec4(Light, 1.0);
}
