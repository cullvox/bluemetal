#version 460

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec3 inTangent;
layout(location=3) in vec2 inTextureCoordinates;

layout(location = 0) out uint objectID;

struct InstanceData {
    mat4 instance;
    vec4 position;
};

layout(set = 0, binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 projection;
    vec2 resolution;
    vec2 mouse;
    float time;
    float dt;
} globals;

layout(set = 1, binding = 0) uniform Uniform {
    float value;
} material;

layout(std140, set = 2, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
} instanceBuffer;

layout(push_constant) uniform DrawConstants
{
    InstanceData objectInstance;
    ivec4 useInstanceBuffer;
    uint objectID;
} drawConstants;

void main()
{
    objectID = drawConstants.objectID;

    InstanceData instance;
    if (drawConstants.useInstanceBuffer.x > 0) {
        instance = instanceBuffer.instances[gl_InstanceIndex];
    } else {
        instance = drawConstants.objectInstance;
    }

    gl_Position = globals.projection * globals.view * instance.instance * vec4(inPosition, 1.0);

}
