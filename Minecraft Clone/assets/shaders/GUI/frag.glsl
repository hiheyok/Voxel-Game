#version 450 core
out vec4 FragColor;

in vec3 color;
in vec2 text;

uniform sampler2D texture0;

void main()
{
    FragColor = vec4(color, 1.0f) * texture(texture0, text );
} 