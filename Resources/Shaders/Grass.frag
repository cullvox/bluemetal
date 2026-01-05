#version 450

layout(location=0) in vec3 inNormal;
layout(location=1) in float inBottomToTop;
layout(location=2) in float inPatchFactor;
layout(location=3) in float inCurrentWindBend;
layout(location=4) in vec3 inColorSmall;
layout(location=5) in vec3 inColorLarge;
layout(location=6) in float inSpecularFactor;

layout(location=0) out vec4 outColor;

void main() {
    float AO = (inBottomToTop - inCurrentWindBend * 0.3); // * windAOEffect;
    // AO_LIGHT_AFFECT = 1.0;
    outColor = vec4(0.6, 0.7, 0.3, 1.0);
    outColor *= AO;
    outColor.a = 1.0;
    // BACKLIGHT = vec3(backLightColor);
    // ROUGHNESS = roughnessFactor;
    // NORMAL = mix(NORMAL, vec3(0.0,1.0,0.0), inBottomToTop);
    //if(!FRONT_FACING) NORMAL = -NORMAL;
    //SPECULAR = specularFactor;
}