#include "Graphics/Config.h"
#include "PresentPass.h"
#include <stdint.h>

namespace bl 
{

PresentPass::PresentPass(Device* device, Swapchain* swapchain)
    : _device(device)
    , _swapchain(swapchain) {
    CreateRenderPass();
    CreateFramebuffers();
}

PresentPass::~PresentPass() {
    DestroyFramebuffers();
    DestroyRenderPass();
}

VkRenderPass PresentPass::Get()  { 
    return _pass;
}

void PresentPass::Recreate(VkExtent2D, uint32_t) {
    DestroyFramebuffers();
    CreateFramebuffers();
}

void PresentPass::Begin(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t index) {
    std::array<VkClearValue, 2> clearValues = {};

    VkClearColorValue value = {};
    value.float32[0] = 0.33f;
    value.float32[1] = 0.34f;
    value.float32[2] = 0.34f;
    value.float32[3] = 1.0f;

    clearValues[0].color = value;
    clearValues[1].depthStencil = VkClearDepthStencilValue{1.0f, 0};

    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.renderPass = _pass;
    beginInfo.framebuffer = _framebuffers[index];
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = (uint32_t)clearValues.size();
    beginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void PresentPass::End(VkCommandBuffer cmd) {
    vkCmdEndRenderPass(cmd);
}

void PresentPass::CreateRenderPass() {
    std::array<VkAttachmentDescription, 1> attachments = {};
    attachments[0].flags = 0;
    attachments[0].format = _swapchain->GetFormat();
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

    std::array colorAttachments =
    { 
        presentAttachmentReference
    };

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

    VK_CHECK(vkCreateRenderPass(_device->Get(), &createInfo, nullptr, &_pass))
}

void PresentPass::DestroyRenderPass() {
    vkDestroyRenderPass(_device->Get(), _pass, nullptr);
}

void PresentPass::CreateFramebuffers() {
    auto extent = _swapchain->GetExtent();
    auto attachments = _swapchain->GetImageViews();
    
    _framebuffers.resize(_swapchain->GetImageCount());

    for (uint32_t i = 0; i < _swapchain->GetImageCount(); i++) {
        VkImageView attachment = attachments[i];

        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = _pass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &attachment;
        createInfo.width = extent.width;
        createInfo.height = extent.height;
        createInfo.layers = 1;
        
        VK_CHECK(vkCreateFramebuffer(_device->Get(), &createInfo, nullptr, &_framebuffers[i]))
    }
}

void PresentPass::DestroyFramebuffers() {
    for (VkFramebuffer fb : _framebuffers) {
        vkDestroyFramebuffer(_device->Get(), fb, nullptr);
    }

    _framebuffers.clear();
}

} // namespace bl