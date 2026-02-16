#pragma once

#include "Math/Math.h"

namespace bl {

struct GlobalUBO // Slot 0, Binding 0
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 projection;
    alignas(8) glm::vec2 resolution;
    alignas(8) glm::vec2 mouse;
    alignas(4) float time;
    alignas(4) float dt;
};

struct InstanceData
{
    glm::mat4 model;
    glm::vec4 position;
};

struct ObjectPC // Model positions use a push constant block
{
    InstanceData data;
    glm::ivec4 useInstanceBuffer;
    uint32_t objectID;
};

} // namespace bl