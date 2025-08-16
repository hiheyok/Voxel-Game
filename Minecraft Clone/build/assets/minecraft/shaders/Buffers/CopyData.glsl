#version 450

layout(local_size_x = 512) in;

layout(std430, binding = 0) buffer Source {
    float srcData[];
};

layout(std430, binding = 1) buffer Destination {
    float dstData[];
};

uniform int srcOffset;
uniform int dstOffset;
uniform int copySize;

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index < copySize) {
        dstData[dstOffset + index] = srcData[srcOffset + index];
    }
}