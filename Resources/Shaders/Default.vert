#version 450

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTextureCoords;

layout(set=0, binding=0) uniform GlobalUniform
{
    float time;
    float dt;
    vec2 resolution;
    vec2 mouse;

    mat4 view;
    mat4 projection;
} global;

layout(push_constant) uniform Constants
{
    mat4 model;
} object;

void main() 
{
    gl_Position = global.projection * global.view * object.model * vec4(inPosition, 1.0);
}