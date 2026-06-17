#include "SceneRenderPass.h"
#include "Graphics/VulkanDevice.h"

#include "Viewport.h"
#include "RenderData.h"

namespace bl {

SceneRenderPass::SceneRenderPass(VulkanDevice* device) 
    : RenderPass()
{

}

SceneRenderPass::~SceneRenderPass()
{
}

void SceneRenderPass::Render(Viewport& vp, RenderData& rd)
{
     auto cmd = rd.GetCommandBuffer();

    // Setup the render pass for dynamic rendering.
    std::array clearColors = {
        VkClearValue { .color = { { 0.96f, 0.97f, 0.96f, 1.0f } } }, // Clear Color
        VkClearValue { .depthStencil = { 1.0f, 0 } }, // Clear Depth
        VkClearValue { .color = { -1, -1, -1, -1 } }
    };

    auto extent = vp.GetExtent();

    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = extent;

    std::array<VkRenderingAttachmentInfo, 2> colorAttachments = {};
    colorAttachments[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachments[0].pNext = nullptr;
    colorAttachments[0].imageView = vp.GetColorImageView();
    colorAttachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachments[0].resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachments[0].resolveImageView = VK_NULL_HANDLE;
    colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[0].clearValue = clearColors[0];

    colorAttachments[1].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachments[1].pNext = nullptr;
    colorAttachments[1].imageView = vp.GetSelectionImageView();
    colorAttachments[1].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachments[1].resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachments[1].resolveImageView = VK_NULL_HANDLE;
    colorAttachments[1].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[1].clearValue = VkClearValue { .color = { -1, -1, -1, -1 } };

    // When using a higher sample count, the image must be resolved from the sampled image.
    if (vp.GetSampleCount() != VK_SAMPLE_COUNT_1_BIT) {
        colorAttachments[0].imageView = vp.GetColorImageView();
        colorAttachments[0].resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        colorAttachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachments[0].resolveImageView = vp.GetColorResolveImageView();

        colorAttachments[1].imageView = vp.GetSelectionImageView();
        colorAttachments[1].resolveMode = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
        colorAttachments[1].resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachments[1].resolveImageView = vp.GetSelectionResolveImageView();
    }

    VkRenderingAttachmentInfo depthAttachment = {};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.pNext = nullptr;
    depthAttachment.imageView = vp.GetDepthImageView();
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
    depthAttachment.resolveImageView = VK_NULL_HANDLE;
    depthAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.clearValue = clearColors[1];

    VkRenderingInfo renderingInfo = {};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.pNext = nullptr;
    renderingInfo.flags = 0;
    renderingInfo.renderArea = renderArea;
    renderingInfo.layerCount = 1;
    renderingInfo.viewMask = 0;
    renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
    renderingInfo.pColorAttachments = colorAttachments.data();
    renderingInfo.pDepthAttachment = &depthAttachment;
    renderingInfo.pStencilAttachment = nullptr;

    VkImageSubresourceRange range = {};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    vkCmdBeginRendering(cmd, &renderingInfo);

    // Set the viewport and scissor sizing for this viewport render.
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewportWithCount(cmd, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = { extent.width, extent.height };
    vkCmdSetScissorWithCount(cmd, 1, &scissor);

    // Write the scenes draw commands to the command buffer.
    _renderData.SetGlobalDescriptorSet(vp.guboData.globalDescriptorSets[_currentFrame]);
    _renderData.SetSampleCount(renderData.sampleCount);
    _renderData.WriteDrawCommands();

    // End the scene geometry pass.
    vkCmdEndRendering(cmd);
}

void SceneRenderPass::RecreateImages(VkExtent2D extent)
{

}


} // namespace bl