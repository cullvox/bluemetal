#pragma once

#include "Graphics/Renderer/RenderPass.h"
#include "Graphics/Window/Swapchain.h"

namespace bl 
{

class PresentPass : public RenderPass 
{
public:
    PresentPass(Device& device, Swapchain& swapchain); /** @brief Constructor */
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

    std::shared_ptr<GfxDevice> _device;
    std::shared_ptr<Swapchain> _swapchain;	
    std::vector<VkFramebuffer> _framebuffers;
    VkRenderPass _pass;
};

} // namespace bl
