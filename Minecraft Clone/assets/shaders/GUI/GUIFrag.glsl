#version 450

out vec4 clr;

in vec2 TexUV;

uniform sampler2D GUITexture;

void main() {
    clr = texture(GUITexture, TexUV);
}