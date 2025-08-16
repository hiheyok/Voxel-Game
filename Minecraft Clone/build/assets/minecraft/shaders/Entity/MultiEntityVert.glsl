#version 450 core

#extension GL_ARB_shader_draw_parameters : enable

layout (std430, binding = 1) buffer EntityPosBuffer {
     float EntityPos[3000000];
};
layout (std430, binding = 2) buffer EntityVelBuffer {
     float EntityVel[3000000];
};
layout (std430, binding = 3) buffer EntityAccBuffer {
     float EntityAcc[3000000];
};


layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 UV;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform float TimeDelta;

out vec2 UVa;
out vec3 GlobalPos;

void main() {
    UVa = UV;

    vec3 gPos =  vec3(EntityPos[gl_InstanceID * 3 + 0], EntityPos[gl_InstanceID * 3 + 1], EntityPos[gl_InstanceID * 3 + 2]);
    vec3 gVel =  vec3(EntityVel[gl_InstanceID * 3 + 0], EntityVel[gl_InstanceID * 3 + 1], EntityVel[gl_InstanceID * 3 + 2]);
    vec3 gAcc =  vec3(EntityAcc[gl_InstanceID * 3 + 0], EntityAcc[gl_InstanceID * 3 + 1], EntityAcc[gl_InstanceID * 3 + 2]);

    GlobalPos = pos + gPos + gVel * TimeDelta + gAcc * TimeDelta * TimeDelta * 0.5f;
    gl_Position = projection * view * model * vec4(GlobalPos, 1.f);
}