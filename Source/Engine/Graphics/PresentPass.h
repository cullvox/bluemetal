#pragma once

#include "RenderPass.h"
#include "Swapchain.h"

namespace bl 
{

class PresentPass : public RenderPass 
{
public:
    PresentPass(Device* device, Swapchain* swapchain); /** @brief Constructor */
    ~PresentPass(); /** @brief Destructor */

    virtual VkRenderPass Get() override; /** @brief Returns the lower level Vulkan render pass. */
    virtual void Begin(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) override;
    virtual void End(VkCommandBuffer cmd) override;
    virtual void Recreate(VkExtent2D extent) override;

private:
    void CreateFramebuffers();
    void DestroyFramebuffers();
    void CreateRenderPass();
    void DestroyRenderPass();

    Device* _device;
    Swapchain* _swapchain;	
    VkRenderPass _pass;
    std::vector<VkFramebuffer> _framebuffers;
};

} // namespace bl