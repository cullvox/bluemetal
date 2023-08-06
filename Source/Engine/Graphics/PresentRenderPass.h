#pragma once

#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"

namespace bl
{

class BLUEMETAL_API PresentRenderPass : public RenderPass
{
public:
    PresentRenderPass(GraphicsDevice* pDevice, Swapchain* pSwapchain);
    virtual ~PresentRenderPass();

    virtual VkRenderPass getHandle() override;
    virtual void record(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) override;
    virtual void resize(VkExtent2D extent) override;

private:
    void createFramebuffers();
    void destroyFramebuffers();
    void createRenderPass();
    void destroyRenderPass();

    GraphicsDevice*             m_pDevice;
    Swapchain*                  m_pSwapchain;	
    std::vector<VkFramebuffer>  m_swapFramebuffers;
    VkRenderPass                m_pass;
};

} // namespace bl