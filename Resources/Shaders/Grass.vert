#version 450

layout(set=1, binding=0) uniform MaterialUniform
{
    uniform float grassScale;
    varying float bottom_to_top;
    uniform float bladeBendFactor;
    //uniform vec3 color;
    vec3 backLightColor;
    float roughnessFactor;
    float specularFactor;

    //clumping
    float patchScale;
    float miniumGrassScale;
    float maxGrassScale;
    varying float patchFactor;
    // grass color
    vec3 colorSmall;
    vec3 colorLarge;
    // wind

    float windSpeed;
    vec2 windDirectionVector;
    varying float currentWindBend;
    float windAOEffect;

    //player position
    float playerRadius;
    vec3 playerPosition;
} material;

layout(set=1, binding=1) uniform sampler2D noiseSampler;
layout(set=1, binding=2) uniform sampler2D windNoiseTexture;

layout(push_constant) uniform Constants
{
    mat4 model;
} object;

void vertex() {
    //UV map
    bottom_to_top = 1.0 - UV.y;

    //wind
    vec2 windPosition = NODE_POSITION_WORLD.xz / 10.0;
    windPosition -= TIME * windDirectionVector * windSpeed;
    currentWindBend = texture(windNoiseTexture, windPosition).x;
    currentWindBend = windSpeed;
    currentWindBend = bottom_to_top * 2.0;

    mat4 inv_model = inverse(MODEL_MATRIX);
    vec2 local_direction = (inv_model * vec4(windDirectionVector.x, 0.0, windDirectionVector.y, 0.0)).xz;

    VERTEX.xz += currentWindBend * local_direction;

    //player position
    float playerDistance = distance(playerPosition, NODE_POSITION_WORLD);
    float bendFromPlayerFactor = max(playerRadius - playerDistance,0.0) / playerRadius;
    vec2 bendDirection = normalize(playerPosition.xz - NODE_POSITION_WORLD.xz);
    VERTEX.xz -= (inv_model * vec4(bendDirection.x, 0.0, bendDirection.y, 0.0)).xz * bendFromPlayerFactor * bottom_to_top;
    VERTEX.x -= bendFromPlayerFactor * bottom_to_top * .5;

    //bend grass blade
    VERTEX.z += bladeBendFactor * pow(bottom_to_top, 2.0);
    //VERTEX = grassScale;
    patchFactor = texture(noiseSampler, NODE_POSITION_WORLD.xz/patchScale).r;
    //VERTEX= patchFactor;
    VERTEX *= mix(miniumGrassScale, maxGrassScale, patchFactor);


}
