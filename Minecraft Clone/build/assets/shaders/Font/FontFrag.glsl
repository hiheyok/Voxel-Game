#version 450

out vec4 FinalColor;

in vec2 TexUV;
in vec3 clr;

uniform sampler2D ascii_font;

void main() {
    vec4 Tex = texture(ascii_font, TexUV);

    FinalColor = vec4(clr, Tex.a);
}