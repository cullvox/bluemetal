#ifndef VIEWPORT_GLSL
#define VIEWPORT_GLSL

layout(set = 0, binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 projection;
    vec2 resolution;
    vec2 mouse;
    float time;
    float dt;
    uint colorSpace;
} viewport;

#endif // VIEWPORT_GLSL