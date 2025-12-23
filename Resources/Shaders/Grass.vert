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

layout(set = 1, binding = 0) uniform MaterialUniform
{
    // Factors
    float grassScale;
    float bladeBendFactor;
    //uniform vec3 color;
    vec3 backLightColor;
    float roughnessFactor;
    float specularFactor;

    // Clumping
    float patchScale;
    float miniumGrassScale;
    float maxGrassScale;

    // Grass Color
    vec3 colorSmall;
    vec3 colorLarge;

    // Wind
    float windSpeed;
    float windSway;
    float windScale;
    vec2 windDirectionVector;
    float windAOEffect;

    // Player Position
    float playerRadius;
    vec3 playerPosition;
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
    bool useInstanceBuffer;
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
    outSpecularFactor = material.specularFactor;

    InstanceData instance;
    if (drawConstants.useInstanceBuffer) {
        instance = instanceBuffer.instances[gl_BaseInstance];
    } else {
        instance = drawConstants.objectInstance;
    }

    //wind
    vec2 windPosition = instance.position.xz * material.windScale;
    windPosition -= globals.time * material.windDirectionVector * material.windSpeed;
    outCurrentWindBend = texture(windNoiseTexture, windPosition).x;
    outCurrentWindBend *= material.windSway;
    outCurrentWindBend *= outBottomToTop * 2.0;

    //mat4 inverseModel = inverse(instance.model); // Inverse model matrix scaling.

    //vec2 local_direction = (inverseModel * vec4(material.windDirectionVector.x, 0.0, material.windDirectionVector.y, 0.0)).xz;

    vertex.xz += outCurrentWindBend; // * local_direction;

    //player position

    float playerDistance = distance(material.playerPosition, instance.position.xyz);
    float bendFromPlayerFactor = max(material.playerRadius - playerDistance, 0.0) / material.playerRadius;
    vec2 bendDirection = normalize(material.playerPosition.xz - instance.position.xz);
    //gl_Position.xz -= (inverseModel * vec4(bendDirection.x, 0.0, bendDirection.y, 0.0)).xz * bendFromPlayerFactor * outBottomToTop;
    //gl_Position.x -= bendFromPlayerFactor * outBottomToTop * .5;

    //bend grass blade
    //vertex.z += material.bladeBendFactor * pow(outBottomToTop, 2.0);
    //VERTEX = grassScale;
    outPatchFactor = texture(noiseSampler, instance.position.xz / material.patchScale).r;
    //VERTEX= patchFactor;
    vertex *= mix(material.miniumGrassScale, material.maxGrassScale, outPatchFactor);

    gl_Position = globals.projection * globals.view * instance.instance * vec4(vertex, 1.0);
}
