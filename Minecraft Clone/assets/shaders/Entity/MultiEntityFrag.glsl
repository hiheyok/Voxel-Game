#version 450 core
out vec4 FragOut;

in vec2 UVa;

uniform sampler2D EntityTexture;

void main() {
	FragOut = texture(EntityTexture, UVa);
}