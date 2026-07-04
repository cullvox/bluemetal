#ifndef DRAW_CONSTANTS_GLSL
#define DRAW_CONSTANTS_GLSL

#include "InstanceData.glsl"

layout(push_constant) uniform DrawConstants
{
    InstanceData objectInstance;
    ivec4 useInstanceBuffer;
    uint objectID;
} draw;

#endif // DRAW_CONSTANTS_GLSL