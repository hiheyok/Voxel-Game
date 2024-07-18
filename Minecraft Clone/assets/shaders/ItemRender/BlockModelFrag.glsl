#version 450 core
#define MULTIPLIER 0.001953125
out vec4 final;

in float Light;
in float texturePosition;
in vec2 UVa;

uniform sampler2D BlockTexture;

void main() 
{

	int Index = int(floor(texturePosition -  1.f));
	float xIndex = float(Index & 511) * MULTIPLIER;
	float yIndex = floor(Index >> 9) * MULTIPLIER;

	vec2 NormalizedTexCoord = UVa * MULTIPLIER;
	NormalizedTexCoord += vec2(xIndex,  yIndex);

	vec4 texture_ =  texture(BlockTexture, NormalizedTexCoord);

	vec4 color = (texture_ * vec4(Light, Light, Light, 1.0f));
	
	final = color;
	
}