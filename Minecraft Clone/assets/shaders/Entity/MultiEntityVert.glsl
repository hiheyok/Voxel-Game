#version 450 core

#extension GL_ARB_shader_draw_parameters : enable

layout (std430, binding = 2) buffer EntityPosBuffer {
     vec3 EntityPos[3333333];
};

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(pos + EntityPos[gl_InstanceID], 1.f);
}