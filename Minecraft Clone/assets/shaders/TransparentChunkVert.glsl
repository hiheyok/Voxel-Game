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
out float z_val;

uniform mat4 model; //delete
uniform mat4 projection;
uniform mat4 view;

uniform float far;
uniform float near;

int xDataBitOffset = 0;
int yDataBitOffset = 5;
int zDataBitOffset = 10;
int blockShadingBitOffset = 15;
int textureBitOffset = 20;

float dataToFloat(int index, int size) {
    return (((1u << size) - 1u) & (data >> index));
}

float tdataToFloat(int index, int size) {
    return (((1u << size) - 1u) & (tdata >> index));
}

float getZ(float far1, float near1, float z) {
    float a = (far1 + near1) / (far1 - near1);
    float b = (-2 * far1 * near1) / (far1 - near1);

    return (1 / z) * b + a;

}

void main()
{
    
    vec3 pos = vec3(dataToFloat(xDataBitOffset, 5),dataToFloat(yDataBitOffset, 5),dataToFloat(zDataBitOffset, 5));
    texturePosition = tdataToFloat(textureBitOffset, 5);
    textureSize = vec2(tdataToFloat(0, 5),tdataToFloat(10, 5));
    float light = dataToFloat(blockShadingBitOffset, 5);

    vec3 VerticePos = vec3(pos.x + (ChunkPos[0 + (3 * gl_DrawIDARB)]* 16),pos.y + (ChunkPos[1 + (3 * gl_DrawIDARB)] * 16),pos.z + (ChunkPos[2 + (3 * gl_DrawIDARB)] * 16));

    poss = VerticePos;
    lights = light / 16.f;


    gl_Position = view * vec4(VerticePos, 1.f);

    z_val = getZ(far, near, gl_Position.w);

    gl_Position = projection * gl_Position;
    FragPos = vec3(model * vec4(VerticePos, 1.0));
  
}