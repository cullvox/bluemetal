#pragma once

#include "Graphics/Device.h"

namespace bl
{

static inline uint32_t maxSystems = 128;

class System
{
public:
    System() = default;
    virtual ~System() = default;

    void onUpdate(float deltaTime) {}
    void onRender(VkCommandBuffer cmd) {}
};

} // namespace bl
