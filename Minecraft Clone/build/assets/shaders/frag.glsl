#version 450 core
const vec3 kSkyColor = vec3(0.46274509803f, 0.65882352941f,1.0f);

out vec4 final;

flat in uint texture_id;
in vec3 global_vert;
in vec2 texture_uv;
in float sky_light;
in float block_light;
in vec4 color;

uniform vec3 camPos;
uniform float RenderDistance;
uniform float VerticalRenderDistance;
uniform sampler2D BlockTexture;
uniform sampler2D LightMap;
uniform vec3 tintColor;

float near = 0.25; 
float far  = RenderDistance; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

vec4 SampleLighting() {
    // Sample the middle of the pixel
    // 16 by 16 image
    float block_samp = block_light * (15.f / 16.f) + (1.f / 32.f);
    float sky_samp = sky_light * (15.f / 16.f) + (1.f / 32.f);
    return texture(LightMap, vec2(block_samp, sky_samp));
}

void main() {
    vec3 cam_rel_vert = global_vert - camPos;

    cam_rel_vert.y *= RenderDistance / VerticalRenderDistance;

    float depth = dot(cam_rel_vert, cam_rel_vert) / pow(RenderDistance,2);
    depth = depth * depth;
    depth = 1 - depth;

    vec4 block_texture =  texture(BlockTexture, texture_uv);
    vec4 light_map =  SampleLighting();

    if (block_texture.a == 0.0f)
        discard;

    vec4 block_color = block_texture  * color * light_map;
    
    if (depth < -0.1f) {
        discard;
    } else {
        depth = depth * float((depth >= 0.0f)); // Set to 0 if depth is negative
        
        float depth_r = block_color.r - ((1 - depth) * (block_color.r - kSkyColor.r));
        float depth_g = block_color.g - ((1 - depth) * (block_color.g - kSkyColor.g));
        float depth_b = block_color.b - ((1 - depth) * (block_color.b - kSkyColor.b));
        final = vec4(depth_r, depth_g, depth_b, block_texture.w);
    }
    
}