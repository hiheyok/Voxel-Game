#version 450 core

#extension GL_ARB_shader_draw_parameters : enable


layout (location = 0) in uint data;
layout (location = 1) in uint tdata;

layout (std430, binding = 2) buffer ChunkPosBuffer {
     int ChunkPos[100000];
};

out float texturePosition;
out float lights;
out vec3 FragPos;
out vec3 poss;
out vec2 textureSize;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform int TextureAimIndex;

int xDataBitOffset = 0;
int yDataBitOffset = 5;
int zDataBitOffset = 10;
int blockShadingBitOffset = 15;
int textureBitOffset = 10;
int NumTextureBitOffset = 22;

float dataToFloat(int index, int size) {
    return (((1u << size) - 1u) & (data >> index));
}

float tdataToFloat(int index, int size) {
    return (((1u << size) - 1u) & (tdata >> index));
}

void main()
{
    int NumTexture = int(tdataToFloat(NumTextureBitOffset, 12));
    vec3 pos = vec3(dataToFloat(xDataBitOffset, 5),dataToFloat(yDataBitOffset, 5),dataToFloat(zDataBitOffset, 5));
    texturePosition = tdataToFloat(textureBitOffset, 12) + (TextureAimIndex % NumTexture);
    textureSize = vec2(tdataToFloat(0, 5),tdataToFloat(5, 5));
    float light = dataToFloat(blockShadingBitOffset, 5);

    vec3 VerticePos = vec3(pos.x + (ChunkPos[0 + (3 * gl_DrawIDARB)]* 16),pos.y + (ChunkPos[1 + (3 * gl_DrawIDARB)] * 16),pos.z + (ChunkPos[2 + (3 * gl_DrawIDARB)] * 16));

    poss = VerticePos;
    lights = light / 16.f;
    gl_Position = projection * view * model * vec4(VerticePos, 1.f);
    FragPos = vec3(model * vec4(VerticePos, 1.0));
}