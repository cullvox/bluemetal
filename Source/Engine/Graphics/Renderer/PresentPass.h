#pragma once

#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"

namespace bl {

class BLUEMETAL_API PresentPass : public RenderPass {
public:

    /** @brief Constructor */
    PresentPass(std::shared_ptr<GfxDevice> device, std::shared_ptr<Swapchain> swapchain);
    
    /** @brief Destructor */
    ~PresentPass();

    /** @brief Returns the lower level Vulkan render pass. */
    virtual VkRenderPass get() override;

    /** @brief Records a command to the */
    virtual void begin(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) override;

    virtual void end(VkCommandBuffer cmd) override;
    virtual void recreate(VkExtent2D extent) override;

private:
    void createFramebuffers();
    void destroyFramebuffers();
    void createRenderPass();
    void destroyRenderPass();

    std::shared_ptr<GfxDevice>      _device;
    std::shared_ptr<Swapchain>   _swapchain;	
    std::vector<VkFramebuffer>      _swapFramebuffers;
    VkRenderPass                    _pass;
};

} // namespace bl
