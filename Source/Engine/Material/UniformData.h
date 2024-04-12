#pragma once

#include "Precompiled.h"

struct GlobalUBO // Slot 0, Binding 0
{
    float time;
    float dt;
    glm::ivec2 resolution;
    glm::ivec2 mouse;

    glm::mat4 view;
    glm::mat4 projection;
};

struct MaterialBuffer // Slot 1, Binding 0
{
    
};

// 5 Texture Binds available below
// Bindings in order, Albedo = 1, Normal = 2, Roughness = 3, AmbientOcclusion = 4, Displacement = 5 

struct ObjectUBO // Slot 2, Binding 0
{
    glm::mat4 model;
};

