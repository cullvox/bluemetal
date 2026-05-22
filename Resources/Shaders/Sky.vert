#version 460

layout(set = 0, binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 projection;
    vec2 resolution;
    vec2 mouse;
    float time;
    float dt;
} globals;

layout(location = 0) out float outTime;
layout(location = 1) out vec3 outEyeDirection;
layout(location = 2) out vec2 outViewportSize;
layout(location = 3) out mat4 outInverseProjection;

void main() {
    mat4 inverseProjection = inverse(globals.projection);
    mat4 inverseView = inverse(globals.view);
    
    // Generates a full-screen triangle (-1 to 1)
    // 6 indices define 2 triangles forming a full-screen quad [-1, 1]
    vec2 positions[6] = vec2[](
        vec2(-1.0, -1.0), // Top-Left
        vec2(-1.0,  1.0), // Bottom-Left
        vec2( 1.0, -1.0), // Top-Right
        vec2( 1.0, -1.0), // Top-Right
        vec2(-1.0,  1.0), // Bottom-Left
        vec2( 1.0,  1.0)  // Bottom-Right
    );

    // UV coordinates ranging from 0.0 to 1.0
    vec2 uvs[6] = vec2[](
        vec2(0.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
    );

    vec2 uv = uvs[gl_VertexIndex];

    gl_Position = vec4(positions[gl_VertexIndex], 1.0, 1.0); // Z = 1.0 puts it in the background

    vec4 clip = vec4(positions[gl_VertexIndex], 1.0, 1.0);
    vec4 viewRay = inverseProjection * clip;
    viewRay /= viewRay.w;

    // Calculate view direction in world space
    vec4 target = inverseProjection * vec4(uv, 1.0, 1.0);
    outEyeDirection = mat3(inverseView) * viewRay.xyz;
    outTime = globals.time;
    outViewportSize = globals.resolution;
}
