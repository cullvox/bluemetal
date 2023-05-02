#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Export.h"

using blRenderPassFunction = std::function<void(VkCommandBuffer)>;

class BLOODLUST_RENDER_API blRenderPass
{
public:
    blRenderPass() = default;
    virtual ~blRenderPass() = default; 

    virtual vk::RenderPass getRenderPass() const noexcept = 0;
    virtual void resize(vk::Extent2D extent) = 0;
    virtual void record(vk::CommandBuffer cmd, vk::Rect2D renderArea, 
                    uint32_t index) = 0;
};