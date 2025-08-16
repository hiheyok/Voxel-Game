#version 450

layout(local_size_x = 512) in;

layout(std430, binding = 0) buffer Buffer {
    float data[];
};

uniform int srcOffset;
uniform int dstOffset;
uniform int copySize;

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index < copySize) {
        data[dstOffset + index] = data[srcOffset + index];
    }
}