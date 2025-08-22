#version 450 core

#extension GL_ARB_shader_draw_parameters : enable
const float kMultiplier = 0.001953125;
const uint kColorMask = 255;
const uint kLightMask = 15;

layout (location = 0) in vec3 vertices_positions;
layout (location = 1) in uint color_data;
layout (location = 2) in vec2 uv;
layout (location = 3) in uint light;

layout (std430, binding = 2) buffer ChunkPosBuffer {
     int chunk_pos[];
};

out uint texture_id;
out vec3 global_vert;
out vec2 texture_uv;
out float sky_light;
out float block_light;
out vec4 color;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
    global_vert = vertices_positions + vec3(chunk_pos[0 + (3 * gl_DrawIDARB)], chunk_pos[1 + (3 * gl_DrawIDARB)], chunk_pos[2 + (3 * gl_DrawIDARB)]) * 16;
    // Unpack the 8 bit colors
    float r = float((color_data >> 0) & 255) / 255.f;
    float g = float((color_data >> 8) & 255) / 255.f;
    float b = float((color_data >> 16) & 255) / 255.f;
    float a = float((color_data >> 24) & 255) / 255.f;
    color = vec4(r, g, b, a);

    texture_uv = uv;

    sky_light = float(light & kLightMask) / 15.f;
    block_light = float((light >> 4) & kLightMask) / 15.f;

    gl_Position = projection * view * model * vec4(global_vert, 1.f);
}
