#version 450 core

out vec4 final;

in float Light;
in float texturePosition;
in vec2 UVa;

uniform sampler2DArray BlockTexture;

void main() 
{
	vec4 texture_ =  texture(BlockTexture, vec3(UVa, texturePosition - 1));

	vec4 color = (texture_ * vec4(Light, Light, Light, 1.0f));
	
	final = color;
	
}