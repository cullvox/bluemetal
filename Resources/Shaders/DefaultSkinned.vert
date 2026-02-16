#version 460

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec3 inTangent;
layout(location=3) in vec2 inTextureCoordinates;
layout(location=4) in ivec4 inBoneIndices;
layout(location=5) in vec4 inBoneWeights;

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

layout(set=1, binding=0) uniform MaterialUniform
{
    bool useTriplanar;
} material;

layout(set=2, binding=0) readonly buffer BoneBuffer
{
    mat4 bones[];
} bones;

struct InstanceData {
    mat4 model;
    vec4 position;
};

layout(push_constant) uniform Constants
{
    InstanceData object;
    ivec4 useInstanceBuffer;
    uint objectID;
} pushConstants;

void main()
{
    vec4 worldPos = pushConstants.object.model * vec4(inPosition, 1.0);
    outPosition = worldPos.xyz;
    outNormal = normalize(mat3(transpose(inverse(pushConstants.object.model))) * inNormal);
    outTextureCoordinates = inTextureCoordinates;

    // Compute the skin matrix.
    mat4 skinMatrix =
        inBoneWeights.x * bones.bones[inBoneIndices.x] +
        inBoneWeights.y * bones.bones[inBoneIndices.y] +
        inBoneWeights.z * bones.bones[inBoneIndices.z] +
        inBoneWeights.w * bones.bones[inBoneIndices.w];

    gl_Position = globals.projection * globals.view * pushConstants.object.model * skinMatrix * vec4(inPosition, 1.0);
}