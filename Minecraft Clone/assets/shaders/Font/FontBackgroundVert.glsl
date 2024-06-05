#version 450

layout (location = 0) in vec2 vertices;
layout (location = 1) in vec2 Origin;
layout (location = 2) in vec4 Color;

uniform float AspectRatio;

out vec4 clr;

void main() {
	clr = Color;
	gl_Position = vec4(vertices.x * AspectRatio + Origin.x, vertices.y + Origin.y, 1.f, 1.f);
}