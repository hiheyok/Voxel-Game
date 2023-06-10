#version 440 core
layout (location = 0) in uint data;
layout (location = 1) in uint tdata;

out float texturePosition;
out float lights;
out vec3 FragPos;
out vec3 poss;
out vec2 textureSize;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform ivec3 ChunkPos;
uniform float ChunkSize;

int xDataBitOffset = 0;
int yDataBitOffset = 9;
int zDataBitOffset = 18;
int blockShadingBitOffset = 27;

int textureBitOffset = 20;

float dataToFloat(int index, int size) {
    return (((1u << size) - 1u) & (data >> index));
}
float tdataToFloat(int index, int size) {
    return (((1u << size) - 1u) & (tdata >> index));
}

void main()
{
    
    vec3 pos = vec3(dataToFloat(xDataBitOffset, 9),dataToFloat(yDataBitOffset, 9),dataToFloat(zDataBitOffset, 9));
    texturePosition = tdataToFloat(textureBitOffset, 8);
    float light = dataToFloat(blockShadingBitOffset, 5);
    
    vec3 pos1 = vec3(pos.x + (ChunkPos.x * ChunkSize),pos.y + (ChunkPos.y * ChunkSize),pos.z + (ChunkPos.z * ChunkSize));

    textureSize = vec2(tdataToFloat(0,10),tdataToFloat(10,10));

   // vec3 pos1 = pos;

    poss = pos1;
    lights = light / 16.f;
    gl_Position = projection * view * model * vec4(pos1, 1.f);
    FragPos = vec3(model * vec4(pos1, 1.0));
}