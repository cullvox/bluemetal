#ifndef INSTANCES_GLSL
#define INSTANCES_GLSL

#include "InstanceData.glsl"

layout(std140, set = 1, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
} instanceBuffer;

#endif // INSTANCES_GLSL