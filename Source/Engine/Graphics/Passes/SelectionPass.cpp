
#include "SelectionPass.h"

#include "Graphics/RenderData.h"
#include "Graphics/VulkanBuffer.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanImageView.h"

#include <array>

namespace bl {

SelectionPass::SelectionPass(VulkanDevice* device, VkExtent2D extent)
    : _device(device)
{
    Resize(extent);
}

SelectionPass::~SelectionPass()
{
}

void SelectionPass::SetMaterial(VulkanMaterialInstance* material)
{
    _material = material;
}

void SelectionPass::Resize(VkExtent2D extent)
{
    _extent = extent;

    VkComponentMapping mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    // Create selection image/buffer
    VkExtent3D extent3D = { extent.width, extent.height, 1 };
    _selectionImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, extent3D, VK_FORMAT_R32_UINT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    _selectionImageView = std::make_unique<VulkanImageView>(_device, _selectionImage.get(), VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32_UINT, mapping, range);
    _selectionBuffer = std::make_unique<VulkanBuffer>(_device, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU, extent.width * extent.height * sizeof(uint32_t), nullptr, true, VMA_ALLOCATION_CREATE_MAPPED_BIT);

    // Depth buffer
    auto physicalDevice = _device->GetPhysicalDevice();
    _depthFormat = physicalDevice->FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, 0);

    _depthImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, extent3D, _depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    _depthImageView = std::make_unique<VulkanImageView>(_device, _depthImage.get(), VK_IMAGE_VIEW_TYPE_2D, _depthFormat, mapping, range);

}

void SelectionPass::Render(RenderData& data)
{
    auto cmd = data.GetCommandBuffer();

    // Transition depth image into a usable state
    VkImageSubresourceRange range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    _selectionImage->Transition(
        cmd,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        0,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        range);

    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    _depthImage->Transition(
        cmd,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        0,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        range);

    vkCmdSetRasterizationSamplesEXT(cmd, VK_SAMPLE_COUNT_1_BIT);

    // Setup rendering info
    VkRect2D renderArea = {};
    renderArea.offset = { 0, 0 };
    renderArea.extent = _extent;

    VkRenderingAttachmentInfo colorAttachment = {};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.pNext = nullptr;
    colorAttachment.imageView = _selectionImageView->Get();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
    colorAttachment.resolveImageView = VK_NULL_HANDLE;
    colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue = { -1, -1, -1, -1 };

    VkRenderingAttachmentInfo depthAttachment = {};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.pNext = nullptr;
    depthAttachment.imageView = _depthImageView->Get();
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
    depthAttachment.resolveImageView = VK_NULL_HANDLE;
    depthAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.clearValue = { 1.0f, 0UL };

    VkRenderingInfo renderingInfo = {};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.pNext = nullptr;
    renderingInfo.flags = 0;
    renderingInfo.renderArea = renderArea;
    renderingInfo.layerCount = 1;
    renderingInfo.viewMask = 0;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    renderingInfo.pDepthAttachment = &depthAttachment;
    renderingInfo.pStencilAttachment = nullptr;

    vkCmdBeginRendering(cmd, &renderingInfo);

    // Draw all objects as their indices
    data.WriteDrawCommands(_material);

    vkCmdEndRendering(cmd);

    // Transition the image into a readable layout.
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    _selectionImage->Transition(
        cmd,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_TRANSFER_READ_BIT,
        range);

    // Copy the image to a local buffer for reading.
    VkBufferImageCopy copy = {};
    copy.bufferOffset = 0;
    copy.bufferRowLength = _extent.width;
    copy.bufferImageHeight = _extent.height;
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.layerCount = 1;
    copy.imageOffset = {0, 0, 0};
    copy.imageExtent = VkExtent3D{_extent.width, _extent.height, 1};

    vkCmdCopyImageToBuffer(cmd, _selectionImage->Get(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, _selectionBuffer->Get(), 1, &copy);

    _selectionImage->Transition(
        cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        range);
}

void SelectionPass::GetColorFormats(std::vector<VkFormat>& formats)
{
    formats = { VK_FORMAT_R32_UINT };
}

VkFormat SelectionPass::GetDepthFormat()
{
    return _depthFormat;
}

VkFormat SelectionPass::GetStencilFormat()
{
    return VK_FORMAT_UNDEFINED;
}

}