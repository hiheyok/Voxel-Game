#version 450

layout (location = 0) in vec2 vertices;
layout (location = 1) in vec2 UV;

uniform float AspectRatio;

out vec2 TexUV;

void main() {
    TexUV = UV;
    gl_Position = vec4(vertices.x * AspectRatio, vertices.y, 1.f, 1.f);
}