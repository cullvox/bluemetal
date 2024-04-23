#pragma once

#include "Device.h"

namespace bl 
{

class RenderPass
{
public:
    RenderPass() = default;
    ~RenderPass() = default; 

    virtual VkRenderPass Get() = 0;
    virtual void Begin(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) = 0; // records the render pass
    virtual void End(VkCommandBuffer cmd) = 0;
    virtual void Recreate(VkExtent2D extent) = 0; // resizes images and framebuffers to match extent 
};

} // namespace bl
