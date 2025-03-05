#version 450

out vec4 FinalColor;

in vec2 TexUV;
in vec3 clr;

uniform sampler2D FontTexture;

void main() {
    vec4 Tex = texture(FontTexture, TexUV);

    FinalColor = vec4(clr, Tex.a);
}