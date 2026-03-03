#version 460

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inColor;
layout(location=2) in float inPointSize;

layout(location=0) out vec3 outColor;

layout(set=0, binding=0) uniform GlobalUniform
{
    mat4 view;
    mat4 projection;
    vec2 resolution;
    vec2 mouse;
    float time;
    float dt;
} globals;

struct InstanceData {
    mat4 instance;
};

layout(std140, set = 1, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
} instanceBuffer;

void main()
{
    outColor = inColor;

    gl_Position = globals.projection * globals.view * vec4(inPosition, 1.0);
    gl_PointSize = inPointSize;
}

