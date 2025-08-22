#version 450 core

layout (location = 0) in vec3 in_vertices;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in float in_light;

out float light;
out vec2 uv;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    uv = in_uv;
    light = in_light / 15.f;
    gl_Position = projection * view * model * vec4(in_vertices, 1.f);
}