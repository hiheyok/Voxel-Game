#version 440 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform vec2 Resolution;

void main()
{ 
    vec2 uv = gl_FragCoord.xy / Resolution.xy;

    uv *=  1.0 - uv.yx;

    float vig = uv.y * uv.x * 100;

    vig = pow(vig, 0.125) + 0.1f;

    if (vig > 1.f) {
        vig = 1.f;
    }


    FragColor = vec4(vig,vig,vig, 1)* texture(screenTexture, TexCoords);
}