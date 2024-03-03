#version 450 core

#extension GL_ARB_shader_draw_parameters : enable

layout (std430, binding = 2) buffer EntityPosBuffer {
     float EntityPos[10000000];
};

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 UV;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec2 UVa;
out vec3 GlobalPos;

void main() {
    UVa = UV;
    GlobalPos = pos + vec3(EntityPos[gl_InstanceID * 3 + 0], EntityPos[gl_InstanceID * 3 + 1], EntityPos[gl_InstanceID * 3 + 2]);
    gl_Position = projection * view * model * vec4(GlobalPos, 1.f);
}