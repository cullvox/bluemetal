#version 450

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec3 inTangent;
layout(location=3) in vec2 inTextureCoordinates;

layout(location=0) out vec3 outPosition;
layout(location=1) out vec2 outTextureCoordinates;
layout(location=2) out vec3 outNormal;

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

layout(set=2, binding=0) uniform MaterialUniform
{
    bool useTriplanar;
} material;

layout(push_constant) uniform Constants
{
    InstanceData objectInstance;
    ivec4 useInstanceBuffer;
    uint objectID;
} object;

void main()
{
    vec4 worldPos = object.objectInstance.instance * vec4(inPosition, 1.0);
    outPosition = worldPos.xyz;
    outNormal = normalize(mat3(transpose(inverse(object.objectInstance.instance))) * inNormal);
    outTextureCoordinates = inTextureCoordinates;

    gl_Position = globals.projection * globals.view * object.objectInstance.instance * vec4(inPosition, 1.0);
}