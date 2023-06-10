#version 440 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 color_;
layout(location = 2) in vec2 textCoords;


out vec3 color;
out vec2 text;

uniform mat4 proj;
uniform vec3 clr;

void main() {
    color = color_;
    text = textCoords;
    gl_Position = proj * vec4(aPos, 0.0f, 1.0f);
}