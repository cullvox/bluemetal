#include "Render/BasicRenderer/BasicPresentRenderPass.hpp"

blBasicPresentRenderPass::blBasicPresentRenderPass(
		std::shared_ptr<const blRenderDevice> renderDevice,
        std::shared_ptr<const blSwapchain> swapchain,
		const blRenderPassFunction& func)
    : blRenderPass(renderDevice, func)
    , _swapchain(swapchain)
{

    const VkAttachmentDescription presentAttachment{
        .flags = 0,
        .format = _swapchain->getFormat(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    
    const VkAttachmentReference presentAttachmentRef{
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkSubpassDescription subpass{
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &presentAttachmentRef,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    const VkRenderPassCreateInfo passCreateInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = 1,
        .pAttachments = &presentAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 0,
        .pDependencies = nullptr
    };

    if (vkCreateRenderPass(
            _renderDevice->getDevice(), 
            &passCreateInfo, 
            nullptr, 
            &_pass)
        != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create the present render pass!");
    }

    createFramebuffers();
}

blBasicPresentRenderPass::~blBasicPresentRenderPass()
{
    destroyFramebuffers();

    vkDestroyRenderPass(_renderDevice->getDevice(), _pass, nullptr);
}

void blBasicPresentRenderPass::createFramebuffers()
{
    const std::vector<VkImageView> attachments = _swapchain->getImageViews();
    const blExtent2D extent = _swapchain->getExtent();

    _swapFramebuffers.resize(attachments.size());

    for (size_t i = 0; i < attachments.size(); i++)
    {
        const VkFramebufferCreateInfo framebufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = _pass,
            .attachmentCount = (uint32_t)attachments.size(),
            .pAttachments = attachments.data(),
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };

        if (vkCreateFramebuffer(
                _renderDevice->getDevice(), 
                &framebufferCreateInfo, 
                nullptr, 
                &_swapFramebuffers[i])
            != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create a present render pass framebuffer!");
        }
    }
}

void blBasicPresentRenderPass::destroyFramebuffers()
{
    for (int i = 0; i < _swapFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(_renderDevice->getDevice(), _swapFramebuffers[i], nullptr);
    }

    _swapFramebuffers.clear();
}

void blBasicPresentRenderPass::resize(blExtent2D extent)
{
    destroyFramebuffers();
    createFramebuffers();
}

void blBasicPresentRenderPass::record(VkCommandBuffer cmd, uint32_t index)
{

}