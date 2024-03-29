#pragma once

#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"

namespace bl {

class BLUEMETAL_API PresentPass : public RenderPass {
public:
    PresentPass(GraphicsDevice* pDevice, Swapchain* pSwapchain);
    virtual ~PresentPass();

    virtual VkRenderPass getHandle() override;
    virtual bool record(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) override;
    virtual bool resize(VkExtent2D extent) override;

private:
    void createFramebuffers();
    void destroyFramebuffers();
    void createRenderPass();
    void destroyRenderPass();

    GraphicsDevice*             _pDevice;
    Swapchain*                  _pSwapchain;	
    std::vector<VkFramebuffer>  _swapFramebuffers;
    VkRenderPass                _pass;
};

} // namespace bl
