#pragma once

#include "Render/Device.h"

namespace bl
{

class System
{
public:
    System() = default;
    virtual ~System() = default;

    void onUpdate(float deltaTime) {}
    void onRender(VkCommandBuffer cmd) {}
};

} // namespace bl
