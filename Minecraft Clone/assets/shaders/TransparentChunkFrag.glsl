#version 440 core

out vec4 final;

in float lights;
in float texturePosition;
in vec3 FragPos;  
in vec3 poss;
in vec2 textureSize;

uniform vec3 camPos;
uniform vec3 lightPos;
uniform float RenderDistance;
uniform sampler2DArray textureArray;

float near = 0.001; 
float far  = RenderDistance; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() 
{

	float tsx = textureSize.x;
	float tsy = textureSize.y;
		

	vec4 texture_ =  texture(textureArray, vec3(vec2(tsx,tsy).xy, texturePosition - 1));
	
	final = texture_ * vec4(lights ,lights, lights,1.0);
	
	
}