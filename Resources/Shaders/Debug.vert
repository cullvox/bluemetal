#version 460

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inColor;
layout(location=2) in float inPointSize;
layout(location=3) in int inShapeType;

layout(location=0) out vec3 outColor;
layout(location=1) out int outShapeType;
layout(location=2) out float outPointSize;

layout(set=0, binding=0) uniform GlobalUniform
{
    mat4 view;
    mat4 projection;
    vec2 resolution;
    vec2 mouse;
    float time;
    float dt;
} globals;

void main()
{
    outColor = inColor;
    outShapeType = inShapeType;
    outPointSize = inPointSize;

    gl_Position = globals.projection * globals.view * vec4(inPosition, 1.0);
    gl_PointSize = inPointSize;
}

