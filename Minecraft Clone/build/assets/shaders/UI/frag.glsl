#version 450 core

#extension GL_ARB_gpu_shader_int64 : enable

#extension GL_ARB_bindless_texture : require 

out vec4 out_frag_clr;
  
in vec4 out_rgb;
flat in int tex_idx;
in vec2 uv;

layout (std430, binding = 0) buffer texture_handles {
  uint64_t textures[];
};

void main() { 
  vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

  if (tex_idx != -1) {
    uint64_t handle = textures[tex_idx];
    sampler2D tex = sampler2D(handle);
    color = texture(tex, uv);
  }

  color *= out_rgb;
  out_frag_clr = color;
}
