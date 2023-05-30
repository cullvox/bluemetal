#include "Render/Renderer/PresentRenderPass.hpp"

blPresentRenderPass::blPresentRenderPass(
		std::shared_ptr<const blRenderDevice> renderDevice,
        std::shared_ptr<const blSwapchain> swapchain,
		const blRenderPassFunction& func)
    : _renderDevice(renderDevice)
    , _swapchain(swapchain)
    , _func(func)
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

    _pass = _renderDevice->getDevice()
        .createRenderPassUnique(renderPassCreateInfo);

    createFramebuffers();
}

blPresentRenderPass::~blPresentRenderPass()
{
    destroyFramebuffers();
}

vk::RenderPass blPresentRenderPass::getRenderPass() const noexcept
{
    return _pass.get();
}

void blPresentRenderPass::resize(vk::Extent2D extent)
{
    destroyFramebuffers();
    createFramebuffers();
}

void blPresentRenderPass::record(vk::CommandBuffer cmd, vk::Rect2D renderArea, uint32_t index)
{
    std::array<vk::ClearValue, 2> clearValues;
    const std::array clearColor = { 0.33f, 0.34f, 0.34f, 1.0f };
    clearValues[0].color = vk::ClearColorValue{clearColor};
    clearValues[1].depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

    const vk::RenderPassBeginInfo beginInfo
    {
        getRenderPass(),                // renderPass
        _swapFramebuffers[index].get(), // framebuffer
        renderArea,                     // renderArea
        clearValues,                    // clearValues
    };

    cmd.beginRenderPass(beginInfo, vk::SubpassContents::eInline);
        _func(cmd);
    cmd.endRenderPass();
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
                
            return _renderDevice->getDevice()
                .createFramebufferUnique(framebufferCreateInfo);
        });
}

void blPresentRenderPass::destroyFramebuffers()
{
    _swapFramebuffers.clear();
}