#version 450 core

#extension GL_ARB_shader_draw_parameters : enable
#define OFFSET -16.f

layout (location = 0) in uint data;
layout (location = 1) in uint tdata;

layout (std430, binding = 2) buffer ChunkPosBuffer {
     int ChunkPos[100000];
};

out uint texturePosition;
out float lights;
out vec3 poss;
out vec2 textureSize;
out uint norm;
out uint tintIndex;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

#define xDataBitOffset 0
#define yDataBitOffset 9
#define zDataBitOffset 18
#define faceNormInfoOffset 27
#define tintBitOffset 29

#define textureBitOffset 10
#define lightDataBitOffset 28

float dataToFloat(int index, int size) {
    return float((((1u << size) - 1u) & (data >> index)));
}

float tdataToFloat(int index, int size) {
    return float((((1u << size) - 1u) & (tdata >> index)));
}

uint dataToUINT(int index, int size) {
    return ((((1u << size) - 1u) & (data >> index)));
}

uint tdataToUINT(int index, int size) {
    return uint((((1u << size) - 1u) & (tdata >> index)));
}

void main() {
    vec3 pos = vec3(dataToFloat(xDataBitOffset, 9) + OFFSET,dataToFloat(yDataBitOffset, 9) + OFFSET,dataToFloat(zDataBitOffset, 9) + OFFSET) * 0.0625f;

    texturePosition = tdataToUINT(textureBitOffset, 18);
    textureSize = vec2(tdataToFloat(0, 5),tdataToFloat(5, 5))* 0.0625f;
    norm = dataToUINT(faceNormInfoOffset, 2);
    lights = tdataToFloat(lightDataBitOffset, 4) / 16.f;
    tintIndex  = dataToUINT(tintBitOffset, 3);

    vec3 VerticePos = vec3(pos.x + (ChunkPos[0 + (3 * gl_DrawIDARB)]* 16),pos.y + (ChunkPos[1 + (3 * gl_DrawIDARB)] * 16),pos.z + (ChunkPos[2 + (3 * gl_DrawIDARB)] * 16));

    poss = VerticePos;
    gl_Position = projection * view * model * vec4(VerticePos, 1.f);
}