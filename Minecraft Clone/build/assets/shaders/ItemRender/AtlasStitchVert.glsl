#version 450 core

layout (location = 0) in vec2 Vertices;
layout (location = 1) in vec2 UV;

out vec2 UVa;


void main() {
    UVa = UV;
    gl_Position = vec4(Vertices,1.f,1.f);
}