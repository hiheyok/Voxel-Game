#version 450 core
out vec4 FragOut;

in vec2 UVa;
in vec3 GlobalPos;

uniform sampler2D EntityTexture;
uniform vec3 camPos;
uniform float RenderDistance;
uniform float VerticalRenderDistance;

void main() {

    vec3 skyColor = vec3(0.46274509803f, 0.65882352941f,1.0f);

    vec3 DeltaPos = GlobalPos - camPos;

    DeltaPos.y *= RenderDistance / VerticalRenderDistance;

    float depth = dot(DeltaPos, DeltaPos) / pow(RenderDistance,2);

    depth = depth * depth;
    depth = 1 - depth;

    vec4 tex = (15.f/16.f) * texture(EntityTexture, UVa);

    if (depth < -0.1f) {
        discard;
    } else {
        if (depth < 0.0f) {
            depth = 0;
        }
        
        float depthr = tex.r - ((1 - depth) * (tex.r - skyColor.r));
        float depthg = tex.g - ((1 - depth) * (tex.g - skyColor.g));
        float depthb = tex.b - ((1 - depth) * (tex.b - skyColor.b));
        FragOut = vec4(depthr, depthg, depthb, 1.0f);
    }

}