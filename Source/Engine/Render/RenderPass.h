#pragma once

#include "Device.h"

using blRenderPassFunction = std::function<void(VkCommandBuffer)>;

class BLUEMETAL_API blRenderPass
{
public:
    blRenderPass() = default;
    virtual ~blRenderPass() = default; 

    virtual VkRenderPass getRenderPass() = 0;
    virtual void resize(VkExtent2D extent) = 0; // resizes images, framebuffers to match extent 
    virtual void record(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t index) = 0; // records the render pass
};