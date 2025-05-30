#version 450

layout(location=0) in vec2 inTextureCoordinates;
layout(location=1) in vec3 inFragmentPosition;
layout(location=2) in vec3 inNormal;

layout(location=0) out vec3 outPosition;
layout(location=1) out vec3 outNormal;
layout(location=2) out vec4 outAlbedoSpec;

layout(set=1, layout=0) uniform sampler2D texture_diffuse1;
layout(set=1, layout=1) uniform sampler2D texture_specular1;

layout(set=1, binding=0) uniform MaterialUniform
{
    vec4 color;
} material;

void main() {
    // outColor = vec4(inTextureCoordinates, 0.0, 1.0);
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
}





void main()
{    

}  