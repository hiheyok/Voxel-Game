#version 450 core
#define MULTIPLIER 0.001953125
out vec4 final;

in float light;
in vec2 uv;

uniform sampler2D blocks;

void main() {
    vec4 block_texture =  texture(blocks, uv);
    vec4 color = (block_texture * vec4(light, light, light, 1.0f));
    final = color;
}