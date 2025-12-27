#version 450 core
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_rgb;
layout (location = 3) in int in_tex_idx;

out vec4 out_rgb;
out vec2 uv;
flat out int tex_idx;

uniform vec2 v_res;
uniform vec2 s_res;

void main()
{
    // Transform it form [0, 1] to [-1, 1]
    uv = in_uv;
    vec2 res_delta = s_res / v_res;
    float aspect_delta = res_delta.x / res_delta.y;      
    vec2 pos = (in_pos / v_res) * 2 - 1;
    if (aspect_delta > 1.0f) {
        pos.x /= aspect_delta;
    } else {
        pos.y *= aspect_delta;
    }

    tex_idx = in_tex_idx;

    gl_Position = vec4(pos, 0.0f, 1.0f); 
    out_rgb = in_rgb;
} 
