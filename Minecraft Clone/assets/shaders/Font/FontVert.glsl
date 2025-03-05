#version 450

layout (location = 0) in vec2 vertices;
layout (location = 1) in vec2 Offset;
layout (location = 2) in vec2 Origin;
layout (location = 3) in vec2 UV;
layout (location = 4) in vec3 Color;

uniform float AspectRatio;

out vec2 TexUV;
out vec3 clr;

void main() {
    TexUV = UV;
    clr = Color;
    gl_Position = vec4(vertices.x * AspectRatio + Offset.x* AspectRatio + Origin.x, vertices.y + Offset.y + Origin.y, 1.f, 1.f);
}