#version 450 core

out vec4 final;

in vec2 UVa;

uniform sampler2D ItemTexture;

void main() {
    vec4 Tex = texture(ItemTexture, UVa);

    final = Tex;
}