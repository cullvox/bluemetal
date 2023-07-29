#include "PresentRenderPass.h"

namespace bl
{

PresentRenderPass::PresentRenderPass(std::shared_ptr<Device> device, std::shared_ptr<Swapchain> swapchain, const RenderPassFunction& func)
    : m_device(device)
    , m_swapchain(swapchain)
    , m_func(func)
{

    std::array<VkAttachmentDescription, 1> attachments = {};
    attachments[0].flags = 0;
    attachments[0].format = m_swapchain->getFormat();
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

    if (vkCreateRenderPass(m_device->getHandle(), &createInfo, nullptr, &m_pass) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan present render pass!");
    }

    createFramebuffers();
}

PresentRenderPass::~PresentRenderPass()
{
    destroyFramebuffers();
    vkDestroyRenderPass(m_device->getHandle(), m_pass, nullptr);
}

VkRenderPass PresentRenderPass::getHandle()
{
    return m_pass;
}

void PresentRenderPass::resize(VkExtent2D extent)
{
    destroyFramebuffers();
    createFramebuffers();
}

void PresentRenderPass::record(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t index)
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
    beginInfo.renderPass = m_pass;
    beginInfo.framebuffer = m_swapFramebuffers[index];
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = (uint32_t)clearValues.size();
    beginInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    m_func(cmd);
    
    vkCmdEndRenderPass(cmd);
}

void PresentRenderPass::createFramebuffers()
{
    // Get the image views from the swapchain.
    std::vector<VkImageView> attachments = m_swapchain->getImageViews();
    
    // Get the extent of the swapchain for framebuffer sizes.
    VkExtent2D extent = m_swapchain->getExtent();

    m_swapFramebuffers.resize(attachments.size());

    // Create some basic info for the loop and fill in the value needed in the loop.
    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.renderPass = m_pass;
    createInfo.attachmentCount = 1;
    createInfo.width = extent.width;
    createInfo.height = extent.height;
    createInfo.layers = 1;

    // Iterate through all the attachments to create their framebuffer.
    uint32_t i = 0;
    for (VkImageView attachment : attachments)
    {
        
        // Set the attachment and create the framebuffer.
        createInfo.pAttachments = &attachment;
        
        if (vkCreateFramebuffer(m_device->getHandle(), &createInfo, nullptr, &m_swapFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create a Vulkan framebuffer!");
        }

        // Iterate the index for the next framebuffer in m_swapFramebuffers.
        i++;
    }
}

void PresentRenderPass::destroyFramebuffers()
{
    for (size_t i = 0; i < m_swapFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(m_device->getHandle(), m_swapFramebuffers[i], nullptr);
    }

    m_swapFramebuffers.clear();
}

} // namespace bl