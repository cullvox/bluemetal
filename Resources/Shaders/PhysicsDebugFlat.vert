#version 460

#include "Viewport.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in float inWidth;


struct InstanceData {
    mat4 instance;
};

layout(std140, set = 1, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
} instanceBuffer;

layout(std140, set = 2, binding = 0) uniform MaterialUniform
{
    vec4 color;
} material;

layout(push_constant) uniform DrawConstants
{
    InstanceData objectInstance;
    ivec4 useInstanceBuffer;
    uint objectID;
} drawConstants;

layout(location = 0) out vec4 outColor;

void main()
{

    InstanceData instance;
    if (drawConstants.useInstanceBuffer.x > 0) {
        instance = instanceBuffer.instances[gl_InstanceIndex];
    } else {
        instance = drawConstants.objectInstance;
    }

    gl_Position = viewport.projection * viewport.view * instance.instance * vec4(inPosition, 1.0);
    outColor = material.color;
}