#pragma once

#include "Device.h"

namespace bl
{

using RenderPassFunction = std::function<void(VkCommandBuffer)>;

class BLUEMETAL_API RenderPass
{
public:
    RenderPass() = default;
    virtual ~RenderPass() = default; 

    virtual VkRenderPass getHandle() = 0;
    virtual void resize(VkExtent2D extent) = 0; // resizes images, framebuffers to match extent 
    virtual void record(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t index) = 0; // records the render pass
};

} // namespace bl