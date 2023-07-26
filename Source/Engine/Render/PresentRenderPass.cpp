#include "PresentRenderPass.h"

namespace bl
{

PresentRenderPass::PresentRenderPass(std::shared_ptr<blDevice> device, std::shared_ptr<blSwapchain> swapchain, const blRenderPassFunction& func)
    : _renderDevice(renderDevice)
    , _swapchain(swapchain)
    , m_func(func)
{

    const std::array<VkAttachmentDescription, 1> attachments =
    {
        { // Present Attachment
            {},                              // flags
            _swapchain->getFormat(),         // format
            vk::SampleCountFlagBits::e1,     // samples
            vk::AttachmentLoadOp::eClear,    // loadOp
            vk::AttachmentStoreOp::eStore,   // storeOp
            vk::AttachmentLoadOp::eDontCare, // stencilLoadOp
            vk::AttachmentStoreOp::eStore,   // stencilStoreOp
            vk::ImageLayout::eUndefined,     // initialLayout
            vk::ImageLayout::ePresentSrcKHR  // finalLayout
        }
    };

    const vk::AttachmentReference presentAttachmentReference
    {
        0,                                          // attachment
        vk::ImageLayout::eColorAttachmentOptimal    // layout 
    };

    const std::array colorAttachments = { presentAttachmentReference };

    const std::array subpasses
    {
        vk::SubpassDescription
        { // Present Subpass
            {},                                 // flags
            vk::PipelineBindPoint::eGraphics,   // pipelineBindPoint
            {},                                 // inputAttachments
            colorAttachments,                   // colorAttachments
            {},                                 // resolveAttachments
            nullptr,                            // pDepthStencilAttachment
            {},                                 // preserveAttachments
        }
    };

    const vk::RenderPassCreateInfo renderPassCreateInfo
    {
        {},             // flags
        attachments,    // attachments
        subpasses,      // subpasses
        {},             // dependencies
    };

    m_pass = _renderDevice->getHandle().createRenderPassUnique(renderPassCreateInfo);

    createFramebuffers();
}

PresentRenderPass::~PresentRenderPass()
{
    destroyFramebuffers();
}

vk::RenderPass PresentRenderPass::getRenderPass() const noexcept
{
    return m_pass.get();
}

void PresentRenderPass::resize(vk::Extent2D extent)
{
    destroyFramebuffers();
    createFramebuffers();
}

void PresentRenderPass::record(vk::CommandBuffer cmd, vk::Rect2D renderArea, uint32_t index)
{
    std::array<vk::ClearValue, 2> clearValues;
    const std::array clearColor = { 0.33f, 0.34f, 0.34f, 1.0f };
    clearValues[0].color = vk::ClearColorValue{clearColor};
    clearValues[1].depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

    const vk::RenderPassBeginInfo beginInfo
    {
        getRenderPass(),                // renderPass
        m_swapFramebuffers[index].get(), // framebuffer
        renderArea,                     // renderArea
        clearValues,                    // clearValues
    };

    cmd.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
        m_func(cmd);
    cmd.endRenderPass();
}

void PresentRenderPass::createFramebuffers()
{
    const std::vector<vk::ImageView> attachments = m_swapchain->getImageViews();
    const blExtent2D extent = m_swapchain->getExtent();

    std::transform(
        attachments.begin(), attachments.end(),
        std::back_inserter(m_swapFramebuffers),
        [&](vk::ImageView imageView)
        {
            const std::array imageViews = { imageView }; 
            const vk::FramebufferCreateInfo framebufferCreateInfo
            {
                {},
                getRenderPass(),
                imageViews,
                extent.width,
                extent.height,
                1
            };
                
            return m_device->getHandle().createFramebufferUnique(framebufferCreateInfo);
        });
}

void PresentRenderPass::destroyFramebuffers()
{
    m_swapFramebuffers.clear();
}

} // namespace bl