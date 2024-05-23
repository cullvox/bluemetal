#pragma once

#include "Device.h"

namespace bl 
{

class RenderPass
{
public:
    RenderPass() = default; /** @brief Default Constructor. (noop)*/
    ~RenderPass() = default; /** @brief */

    virtual VkRenderPass Get() = 0; /** @brief Returns the underlying Vulkan render pass object for pipelines/materials. */
    virtual void Recreate(VkExtent2D extent) = 0; /** @brief When the swapchain image size changes framebuffers need to reflect that. */ 
    virtual void Begin(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) = 0; /** @brief Begins recording the render pass. */
    virtual void End(VkCommandBuffer cmd) = 0; /** @brief Ends the render pass. */
};

} // namespace bl
