#version 450

#include "Viewport.glsl"
#include "Conversions.glsl"
#include "DrawConstants.glsl"

layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTextureCoordinates;
layout(location=2) in vec3 inNormal;

layout(location=0) out vec4 outColor;
layout(location=1) out uint outSelector;


layout(set=2, binding=0) uniform MaterialUniform
{
    bool useTriplanar;
} material;

layout(set=2, binding=1) uniform sampler2D inAlbedo;


const float sharpness = 1.0;

vec4 triplanarTexture(sampler2D tex, vec3 worldPos, vec3 normal, float scale)
{
    // Normalize normal and use absolute value for unbiased weights
    vec3 n = normalize(normal);
    vec3 blend = abs(n);

    // Optional: sharpen blending
    blend = pow(blend, vec3(sharpness));
    blend /= (blend.x + blend.y + blend.z);

    // Project coordinates for each axis
    vec2 xProj = worldPos.yz * scale;
    vec2 yProj = worldPos.xz * scale;
    vec2 zProj = worldPos.xy * scale;

    vec4 xTex = texture(tex, xProj);
    vec4 yTex = texture(tex, yProj);
    vec4 zTex = texture(tex, zProj);

    return xTex * blend.x + yTex * blend.y + zTex * blend.z;
}

void main() 
{
    outSelector = draw.objectID;

    if (material.useTriplanar) {
        vec3 N = normalize(inNormal);
        vec4 col = triplanarTexture(inAlbedo, inPosition, N, 0.1);
        outColor = col;
    }
    else {
        outColor = texture(inAlbedo, inTextureCoordinates);
    }

    outColor = ConvertColorSpace(viewport.colorSpace, outColor);
}
