#version 450 core
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec4 in_rgb;

out vec4 out_rgb;

void main()
{
    // Transform it form [0, 1] to [-1, 1]

    vec2 pos = in_pos * 2 - 1;

    gl_Position = vec4(pos, 0.0f, 1.0f); 
    out_rgb = in_rgb;
} 
