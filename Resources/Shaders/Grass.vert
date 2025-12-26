#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inUV;

layout(set = 0, binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 projection;
    vec2 resolution;
    vec2 mouse;
    float time;
    float dt;
} globals;

layout(std140, set = 1, binding = 0) uniform MaterialUniform
{
    // Factors
    vec4 factors;           // x=grassScale, y=bladeBendFactor, z=roughnessFactor, w=specularFactor

    // Back light color
    vec4 backLightColor;    // xyz=backLightColor, w=padding

    // Clumping
    vec4 clumping;          // x=patchScale, y=minGrassScale, z=maxGrassScale, w=padding

    // Grass Color
    vec4 colorSmall;        // rgb=colorSmall, a=padding
    vec4 colorLarge;        // rgb=colorLarge, a=padding

    // Wind
    vec4 windParams;        // x=windSpeed, y=windSway, z=windScale, w=windAOEffect
    vec4 windDirection;     // xy=windDirectionVector, z=padding, w=padding

    // Player Position
    vec4 playerParams;      // xyz=playerPosition, w=playerRadius
} material;

layout(set = 1, binding = 1) uniform sampler2D noiseSampler;
layout(set = 1, binding = 2) uniform sampler2D windNoiseTexture;

struct InstanceData {
    mat4 instance;
    vec4 position;
};

layout(std140, set = 2, binding = 0) readonly buffer InstanceBuffer {
    InstanceData instances[];
} instanceBuffer;

layout(push_constant) uniform DrawConstants
{
    InstanceData objectInstance;
    ivec4 useInstanceBuffer;
} drawConstants;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out float outBottomToTop;
layout(location = 2) out float outPatchFactor;
layout(location = 3) out float outCurrentWindBend;
layout(location = 4) out vec3 outColorSmall;
layout(location = 5) out vec3 outColorLarge;
layout(location = 6) out float outSpecularFactor;

void main() {
    //UV map
    vec3 vertex = inPosition;

    outBottomToTop = 1.0 - inUV.y;
    outSpecularFactor = material.factors.w; // material.factors.w = specularFactor

    InstanceData instance;
    if (drawConstants.useInstanceBuffer.x > 0) {
        instance = instanceBuffer.instances[gl_InstanceIndex];
    } else {
        instance = drawConstants.objectInstance;
    }

    //wind
    vec2 windPosition = instance.position.xz * material.windParams.z; // materialWindParams.z = windScale
    windPosition -= globals.time * material.windDirection.xy * material.windParams.x; // material.windParams.x = windSpeed
    outCurrentWindBend = texture(windNoiseTexture, windPosition).x;
    outCurrentWindBend *= material.windParams.y; // windParams.y = windSway
    outCurrentWindBend *= outBottomToTop * 2.0;

    mat4 inverseModel = inverse(instance.instance); // Inverse model matrix scaling.

    mat3 rot = mat3(instance.instance);
    vec2 local_direction = (transpose(rot) * vec3(material.windDirection.x, 0.0, material.windDirection.y)).xz;

    vertex.xz += outCurrentWindBend * local_direction;

    //player position

    float playerDistance = distance(material.playerParams.xyz, instance.position.xyz);
    float bendFromPlayerFactor = max(material.playerParams.w - playerDistance, 0.0) / material.playerParams.w;
    vec2 bendDirection = normalize(material.playerParams.xz - instance.position.xz);
    vertex.xz -= (vec4(bendDirection.x, 0.0, bendDirection.y, 0.0)).xz * bendFromPlayerFactor * outBottomToTop;
    vertex.x -= bendFromPlayerFactor * outBottomToTop * .5;

    //bend grass blade
    vertex.z += material.factors.y * pow(outBottomToTop, 2.0);
    //VERTEX = grassScale;

    // material.clumping.x = patchScale
    outPatchFactor = texture(noiseSampler, instance.position.xz / material.clumping.x).r;
    //VERTEX= patchFactor;

    // material.clumping.y = minGrassScale, material.clumping.z = maxGrassScale
    vertex *= mix(material.clumping.y, material.clumping.z, outPatchFactor);

    gl_Position = globals.projection * globals.view * instance.instance * vec4(vertex, 1.0);
    outColorSmall = material.colorSmall.xyz;
}
