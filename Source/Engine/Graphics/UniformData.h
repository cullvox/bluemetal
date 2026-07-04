#pragma once

#include "Math/Math.h"
#include <vulkan/vulkan_core.h>

namespace bl {

struct ViewportUBO // Slot 0, Binding 0
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 projection;
    alignas(8) glm::vec2 resolution;
    alignas(8) glm::vec2 mouse;
    alignas(4) float time;
    alignas(4) float dt;
    VkBool32 bConvertGamma;
};

struct InstanceData
{
    glm::mat4 model;
};

struct ObjectPC // Model positions use a push constant block
{
    InstanceData data;
    glm::ivec4 useInstanceBuffer;
    uint32_t objectID;
};

} // namespace bl