#include "PresentPass.h"
#include "Engine/Engine.h"

#include "imgui/imgui.h"

namespace bl {

PresentPass::PresentPass(std::shared_ptr<GfxDevice> device, std::shared_ptr<Swapchain> swapchain)
    : _device(device)
    , _swapchain(swapchain)
{
    createRenderPass();
    createFramebuffers();
}

PresentPass::~PresentPass()
{
    destroyFramebuffers();
    destroyRenderPass();
}

VkRenderPass PresentPass::get() { return _pass; }

void PresentPass::recreate(VkExtent2D)
{
    destroyFramebuffers();
    createFramebuffers();
}

void PresentPass::begin(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t index)
{
    std::array<VkClearValue, 2> clearValues = {};

    VkClearColorValue value = {};
    value.float32[0] = 0.33f;
    value.float32[1] = 0.34f;
    value.float32[2] = 0.34f;
    value.float32[3] = 1.0f;

    clearValues[0].color = value;
    clearValues[1].depthStencil = VkClearDepthStencilValue(1.0f, 0);

    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.renderPass = _pass;
    beginInfo.framebuffer = _swapFramebuffers[index];
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = (uint32_t)clearValues.size();
    beginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void PresentPass::end(VkCommandBuffer cmd)
{
    vkCmdEndRenderPass(cmd);
}

void PresentPass::createRenderPass()
{
    std::array<VkAttachmentDescription, 1> attachments = {};
    attachments[0].flags = 0;
    attachments[0].format = _swapchain->getFormat();
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference presentAttachmentReference = {};
    presentAttachmentReference.attachment = 0;
    presentAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    std::array colorAttachments = { presentAttachmentReference };

    std::array<VkSubpassDescription, 1> subpasses = {};
    subpasses[0].flags = {};
    subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpasses[0].inputAttachmentCount = 0;
    subpasses[0].pInputAttachments = nullptr;
    subpasses[0].colorAttachmentCount = (uint32_t)colorAttachments.size();
    subpasses[0].pColorAttachments = colorAttachments.data();
    subpasses[0].pResolveAttachments = nullptr;
    subpasses[0].pDepthStencilAttachment = nullptr;
    subpasses[0].preserveAttachmentCount = 0;
    subpasses[0].pPreserveAttachments = nullptr;

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = (uint32_t)attachments.size();
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = (uint32_t)subpasses.size();
    createInfo.pSubpasses = subpasses.data();
    createInfo.dependencyCount = 0;
    createInfo.pDependencies = nullptr;

    VK_CHECK(vkCreateRenderPass(_device->get(), &createInfo, nullptr, &_pass))
}

void PresentPass::destroyRenderPass()
{
    vkDestroyRenderPass(_device->get(), _pass, nullptr);
}

void PresentPass::createFramebuffers()
{
    // Get the image views from the swapchain.
    std::vector<VkImageView> attachments = _swapchain->getImageViews();

    // Get the extent of the swapchain for framebuffer sizes.
    VkExtent2D extent = _swapchain->getExtent();

    _swapFramebuffers.resize(attachments.size());

    // Create some basic info for the loop and fill in the value needed in the loop.
    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.renderPass = _pass;
    createInfo.attachmentCount = 1;
    createInfo.width = extent.width;
    createInfo.height = extent.height;
    createInfo.layers = 1;

    // Iterate through all the attachments to create their framebuffer.
    uint32_t i = 0;
    for (VkImageView attachment : attachments) {

        // Set the attachment and create the framebuffer.
        createInfo.pAttachments = &attachment;
        VK_CHECK(vkCreateFramebuffer(_device->get(), &createInfo, nullptr, &_swapFramebuffers[i]))

        // Iterate the index for the next framebuffer in m_swapFramebuffers.
        i++;
    }
}

void PresentPass::destroyFramebuffers()
{
    for (size_t i = 0; i < _swapFramebuffers.size(); i++) {
        vkDestroyFramebuffer(_device->get(), _swapFramebuffers[i], nullptr);
    }

    _swapFramebuffers.clear();
}

} // namespace bl