#version 450

layout(location=0) in vec2 inTextureCoordinates;

layout(location=0) out vec4 outColor;

layout(set=1, binding=0) uniform MaterialUniform
{
    vec4 color;
} material;

layout(set=1, binding=1) uniform sampler2D image; 

void main() {
    // outColor = vec4(inTextureCoordinates, 0.0, 1.0);
    outColor = texture(image, inTextureCoordinates);
}