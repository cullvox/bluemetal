#version 450

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTextureCoords;


layout(set=0, binding=0) uniform GlobalUBO {
    float time;
    float dt;
    ivec2 resolution;
    ivec2 mouse;

    mat4 view;
    mat4 projection;
} global;

layout(set=1, binding=0) uniform ObjectUBO {
    mat4 model;
} object;

void main() {
    gl_Position = global.projection * global.view * object.model * vec4(inPosition, 1.0);
}