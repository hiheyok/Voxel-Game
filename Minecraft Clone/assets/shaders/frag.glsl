#version 450 core
#define MULTIPLIER 0.001953125

const vec3 skyColor = vec3(0.46274509803f, 0.65882352941f,1.0f);

out vec4 final;

in float lights;
flat in uint texturePosition;
flat in uint norm;
flat in uint tintIndex;
in vec2 textureSize;
in vec3 poss;

uniform vec3 camPos;
uniform float RenderDistance;
uniform float VerticalRenderDistance;
uniform sampler2D BlockTexture;
uniform vec3 tintColor;

float near = 0.25; 
float far  = RenderDistance; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main() {
	vec3 posvec = poss - camPos;

	posvec.y *= RenderDistance / VerticalRenderDistance;

	float depth = dot(posvec, posvec) / pow(RenderDistance,2);
	depth = depth * depth;
	depth = 1 - depth;

	int Index = int(floor(texturePosition -  1.f));
	float xIndex = float(Index & 511) * MULTIPLIER;
	float yIndex = floor(Index >> 9) * MULTIPLIER;

	vec2 NormalizedTexCoord = textureSize * MULTIPLIER;
	NormalizedTexCoord += vec2(xIndex,  yIndex);

	vec3 n = vec3(0.f,0.f,0.f);

	if (norm  == 0) {
		n = vec3(1.f, 0.f, 0.f);
	} else if (norm == 1) {
		n = vec3(0.f, 1.f, 0.f);
	} else if (norm == 2) {
		n = vec3(0.f, 0.f, 1.f);
	}
//
//	vec2 tileUV = vec2(dot(n.zxy, poss), dot(n.yzx, poss));
//
//	vec2 texCoord = vec2(xIndex,yIndex) + textureSize  * MULTIPLIER * fract(tileUV);


	vec4 texture_ =  texture(BlockTexture, NormalizedTexCoord);

	if (tintIndex == 0) {
		texture_.x *=  tintColor.x;
		texture_.y *=  tintColor.y;
		texture_.z *=  tintColor.z;
	}

	if (texture_.a == 0.0f)
		discard;

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
		final = vec4(depthr, depthg, depthb, texture_.w);
	}
	
}