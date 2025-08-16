#version 450 core

#extension GL_ARB_shader_draw_parameters : enable
const float kMultiplier = 0.001953125;
const uint kColorMask = 255;
const uint kTextureMask = (1 << 14) - 1;
const uint kUVMask = (1 << 5) - 1;
const uint kLightMask = (1 << 4) - 1;

layout (location = 0) in vec3 vertices_positions;
layout (location = 1) in uint color_data;
layout (location = 2) in uint tex_uv_light_data;

layout (std430, binding = 2) buffer ChunkPosBuffer {
     int chunk_pos[100000];
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

    // Unpack the texture uv light
    texture_id = (tex_uv_light_data & kTextureMask) - 1;

    // Work on uv coordinates
    float u = float((tex_uv_light_data >> 14) & kUVMask) / 16.f;
    float v = float((tex_uv_light_data >> 19) & kUVMask) / 16.f;
    u *= kMultiplier;
    v *= kMultiplier;
    float u_offset = float(texture_id & 511) * kMultiplier;
    float v_offset = float(texture_id >> 9) * kMultiplier;
    texture_uv = vec2(u + u_offset, v + v_offset);

    // Work on lighting
    sky_light = float((tex_uv_light_data >> 24) & kLightMask) / 15.f;
    block_light = float((tex_uv_light_data >> 28) & kLightMask) / 15.f;

    gl_Position = projection * view * model * vec4(global_vert, 1.f);
}
