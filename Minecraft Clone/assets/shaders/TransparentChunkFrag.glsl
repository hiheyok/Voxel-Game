#version 450 core

out vec4 final;

in float lights;
in float texturePosition;
in vec3 FragPos;  
in vec2 textureSize;
in vec3 poss;

uniform vec3 camPos;
uniform float RenderDistance;
uniform sampler2DArray BlockTexture;

float near = 0.25; 
float far  = RenderDistance; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}
//


void main() 
{
	vec3 skyColor = vec3(0.46274509803f, 0.65882352941f,1.0f);

	vec3 posvec = poss - camPos;

	float depth = dot(posvec, posvec) / pow(RenderDistance,2);

	depth = depth * depth;
	depth = 1 - depth;

	vec4 texture_ =  vec4(texture(BlockTexture, vec3(textureSize.x,textureSize.y, texturePosition - 1)).rgba);

	vec4 color = (texture_ * vec4(lights,lights, lights,1.0f));
	
	if (depth < -0.1f) {
		discard;
	} else {
		if (depth < 0.0f) {
			depth = 0;
		}
		
		float depthr = color.r - ((1 - depth) * (color.r - skyColor.r));
		float depthg = color.g - ((1 - depth) * (color.g - skyColor.g));
		float depthb = color.b - ((1 - depth) * (color.b - skyColor.b));
		final = vec4(depthr, depthg, depthb, texture_.a);
	}
	
}