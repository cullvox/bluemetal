#pragma once

#include "Graphics/RenderPass.h"
#include "Graphics/Swapchain.h"

namespace bl
{

class BLUEMETAL_API PresentRenderPass : public RenderPass
{
public:
    PresentRenderPass(std::shared_ptr<Device> device, std::shared_ptr<Swapchain> swapchain);
    virtual ~PresentRenderPass();

    virtual VkRenderPass getHandle() override;
    virtual void record(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) override;
    virtual void resize(VkExtent2D extent) override;

private:
    void createFramebuffers();
    void destroyFramebuffers();
    void createRenderPass();
    void destroyRenderPass();

    std::shared_ptr<Device>     m_device;
    std::shared_ptr<Swapchain>  m_swapchain;	
    std::vector<VkFramebuffer>  m_swapFramebuffers;
    VkRenderPass                m_pass;
};

} // namespace bl