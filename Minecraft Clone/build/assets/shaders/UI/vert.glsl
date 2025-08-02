#version 450 core
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec4 in_rgb;

out vec4 out_rgb;

void main()
{
    gl_Position = vec4(in_pos, 0.0f, 1.0f); 
    out_rgb = in_rgb;
} 
