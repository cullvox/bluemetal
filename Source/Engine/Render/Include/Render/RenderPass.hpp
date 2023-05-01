#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Export.h"

using blRenderPassFunction = std::function<void(VkCommandBuffer)>;

class BLOODLUST_RENDER_API blRenderPass
{
public:
    blRenderPass() = default;
    virtual ~blRenderPass() = default; 

    virtual vk::RenderPass getRenderPass() const noexcept;
    virtual void resize(blExtent2D extent);
    virtual void record(VkCommandBuffer cmd, uint32_t index);
};