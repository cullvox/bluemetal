#version 460

#include "Viewport.glsl"
#include "Instances.glsl"

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inColor;
layout(location=2) in float inPointSize;

layout(location=0) out vec3 outColor;

void main()
{
    outColor = inColor;

    gl_Position = viewport.projection * viewport.view * vec4(inPosition, 1.0);
    gl_PointSize = inPointSize;
}

