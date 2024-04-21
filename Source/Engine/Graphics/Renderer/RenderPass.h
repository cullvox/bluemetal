#pragma once

#include "Device.h"

namespace bl {

class BLUEMETAL_API RenderPass {
public:
    RenderPass() = default;
    virtual ~RenderPass() = default; 

    virtual VkRenderPass get() = 0;
    virtual void begin(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) = 0; // records the render pass
    virtual void end(VkCommandBuffer cmd) = 0;
    virtual void recreate(VkExtent2D extent) = 0; // resizes images and framebuffers to match extent 
};

} // namespace bl
