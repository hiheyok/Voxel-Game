#version 450 core
out vec4 out_frag_clr;
  
in vec4 out_rgb;

void main() { 
  out_frag_clr = out_rgb;
}
