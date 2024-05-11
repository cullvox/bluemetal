#pragma once

#include "Precompiled.h"

namespace bl 
{

struct GlobalUBO // Slot 0, Binding 0
{
    alignas(4) float time;
    alignas(4) float dt;
    alignas(8) glm::ivec2 resolution;
    alignas(8) glm::ivec2 mouse;

    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 projection;
};

struct MaterialBuffer // Slot 1, Binding 0
{


    // 5 Texture Binds available below
    // Bindings in order, Albedo = 1, Normal = 2, Roughness = 3, AmbientOcclusion = 4, Displacement = 5 
};


struct ObjectPC // Model positions use a push constant block
{
    alignas(16) glm::mat4 model;
};

} // namespace bl