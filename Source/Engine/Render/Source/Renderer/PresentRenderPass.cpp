#include "Render/Renderer/PresentRenderPass.hpp"

blPresentRenderPass::blPresentRenderPass(
		std::shared_ptr<const blRenderDevice> renderDevice,
        std::shared_ptr<const blSwapchain> swapchain,
		const blRenderPassFunction& func)
    : blRenderPass(renderDevice, func)
    , _swapchain(swapchain)
{

    const std::array attachments 
    {
        vk::AttachmentDescription
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

    const std::array subpasses
    {
        vk::SubpassDescription
        { // Present Subpass
            {},                                 // flags
            vk::PipelineBindPoint::eGraphics,   // pipelineBindPoint
            {},                                 // inputAttachments
            { presentAttachmentReference },     // colorAttachments
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

    _pass = _renderDevice->getDevice()
        .createRenderPassUnique(renderPassCreateInfo);

    createFramebuffers();
}

blPresentRenderPass::~blPresentRenderPass()
{
    destroyFramebuffers();
}

void blPresentRenderPass::createFramebuffers()
{
    const std::vector<vk::ImageView> attachments = _swapchain->getImageViews();
    const blExtent2D extent = _swapchain->getExtent();

    std::transform(
        attachments.begin(), attachments.end(),
        std::back_inserter(_swapFramebuffers),
        [&](vk::ImageView imageView)
        {
            const vk::FramebufferCreateInfo framebufferCreateInfo
            {
                {},
                getRenderPass(),
                { imageView },
                extent.width,
                extent.height,
                1
            };
                
            return _renderDevice->getDevice()
                .createFramebufferUnique(framebufferCreateInfo);
        });
}

void blPresentRenderPass::destroyFramebuffers()
{
    _swapFramebuffers.clear();
}

void blPresentRenderPass::resize(blExtent2D extent)
{
    destroyFramebuffers();
    createFramebuffers();
}

void blPresentRenderPass::record(VkCommandBuffer cmd, uint32_t index)
{

}