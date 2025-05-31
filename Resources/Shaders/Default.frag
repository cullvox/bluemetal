#version 450

layout(location=0) in vec2 inTextureCoordinates;

layout(location=0) out vec4 outColor;

layout(set=1, binding=1) uniform sampler2D inAlbedo;

void main() 
{
    outColor = texture(inAlbedo, inTextureCoordinates);
}
