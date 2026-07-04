#version 450

#include "Viewport.glsl"
#include "DrawConstants.glsl"
#include "Instances.glsl"

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec3 inTangent;
layout(location=3) in vec2 inTextureCoordinates;

layout(location=0) out vec3 outPosition;
layout(location=1) out vec2 outTextureCoordinates;
layout(location=2) out vec3 outNormal;

layout(set=2, binding=0) uniform MaterialUniform
{
    bool useTriplanar;
} material;

void main()
{
    vec4 worldPos = draw.objectInstance.instance * vec4(inPosition, 1.0);
    outPosition = worldPos.xyz;
    outNormal = normalize(mat3(transpose(inverse(draw.objectInstance.instance))) * inNormal);
    outTextureCoordinates = inTextureCoordinates;

    gl_Position = viewport.projection * viewport.view * draw.objectInstance.instance * vec4(inPosition, 1.0);
}