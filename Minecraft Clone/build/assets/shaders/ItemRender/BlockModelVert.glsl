#version 450 core

layout (location = 0) in vec3 Vertices;
layout (location = 1) in vec2 UV;
layout (location = 2) in float TexturePosition;
layout (location = 3) in float light;

out float Light;
out float texturePosition;
out vec2 UVa;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    UVa = UV;
    texturePosition = float(TexturePosition);
    Light = light / 16.f;
    gl_Position = projection * view * model * vec4(Vertices, 1.f);
}