#include "Viewport.h"

#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanImageView.h"
#include "ImGui/imgui_internal.h"
#include "Precompiled.h"
#include "RenderData.h"
#include "RenderPass.h"
#include "VulkanSwapchain.h"
#include "RenderData.h"
#include "UniformData.h"
#include "Renderer.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanPhysicalDevice.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace bl {

Viewport::Viewport(Renderer* renderer)
    : _device(renderer->GetDevice())
    , _renderer(renderer)
    , _sampleCount(VK_SAMPLE_COUNT_1_BIT)
    , _extent({0, 0})
{
    // Create the global uniform viewport buffer.
    _globalBuffer = VulkanBufferFrameRing{ _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(bl::ViewportUBO) };

    // Create the viewport descriptor sets to bind later.
    std::vector<VkDescriptorSetLayoutBinding> bindings { 1 };
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    _globalDescriptorSetLayout = _device->AcquireDescriptorSetLayout(bindings);

    std::array<VkDescriptorBufferInfo, VulkanConfig::maxFramesInFlight> descriptorBufferInfos;
    std::array<VkWriteDescriptorSet, VulkanConfig::maxFramesInFlight> descriptorWrites;

    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _globalDescriptorSets[i] = _device->AllocateDescriptorSet(_globalDescriptorSetLayout);

        descriptorBufferInfos[i] = _globalBuffer.GetDescriptorInfo(i);

        descriptorWrites[i] = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = _globalDescriptorSets[i].Get(),
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &descriptorBufferInfos[i],
        };
    }

    _uboData.bConvertGamma = 1;

    vkUpdateDescriptorSets(_device->Get(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

Viewport::Viewport(Renderer* renderer, VkExtent2D extent)
    : Viewport(renderer)
{
    // Set size and create the viewports images.
    SetSize(extent);
    RecreateImages();
}


Viewport::~Viewport()
{
}

void Viewport::RecreateImages()
{
    onPreViewportResized.Broadcast(this);

    if (_colorImage && _colorImageResolved && _selectionImage && _selectionImageResolved && _depthImage)
    {
        // Add images to the deleter queue.
        _renderer->AddToDeletionQueue(std::move(_colorImageView));
        _renderer->AddToDeletionQueue(std::move(_colorImage));



        //_colorImageResolved->

        _renderer->AddToDeletionQueue(std::move(_colorImageResolvedView));
        _renderer->AddToDeletionQueue(std::move(_colorImageResolved));

        _renderer->AddToDeletionQueue(std::move(_selectionImageView));
        _renderer->AddToDeletionQueue(std::move(_selectionImage));
        _renderer->AddToDeletionQueue(std::move(_selectionImageResolvedView));
        _renderer->AddToDeletionQueue(std::move(_selectionImageResolved));

        _renderer->AddToDeletionQueue(std::move(_depthImageView));
        _renderer->AddToDeletionQueue(std::move(_depthImage));
    }

    // Create color image
    VkComponentMapping mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    VkExtent3D extent3d = { _extent.width, _extent.height, 1 };

    VkFormat colorFormat = _renderer->GetViewportColorFormat(), selectionFormat = _renderer->GetViewportSelectionFormat(), depthFormat = _renderer->GetViewportDepthFormat();

    _colorImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, extent3d, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, false, _sampleCount);
    _colorImageView = std::make_unique<VulkanImageView>(_device, _colorImage.get(), VK_IMAGE_VIEW_TYPE_2D, colorFormat, mapping, range);

    _colorImageResolved = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, extent3d, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, false, VK_SAMPLE_COUNT_1_BIT);
    _colorImageResolvedView = std::make_unique<VulkanImageView>(_device, _colorImageResolved.get(), VK_IMAGE_VIEW_TYPE_2D, colorFormat, mapping, range);

    // Create selection buffer images
    _selectionImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, extent3d, selectionFormat, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    _selectionImageView = std::make_unique<VulkanImageView>(_device, _selectionImage.get(), VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32_UINT, mapping, range);

    _selectionImageResolved = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, extent3d, selectionFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, false, _sampleCount);
    _selectionImageResolvedView = std::make_unique<VulkanImageView>(_device, _selectionImageResolved.get(), VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R32_UINT, mapping, range);

    _selectionBuffer = std::make_unique<VulkanBuffer>(_device, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU, _extent.width * _extent.height * sizeof(uint32_t), nullptr, true, VMA_ALLOCATION_CREATE_MAPPED_BIT);

    // Create depth image
    _depthImage = std::make_unique<VulkanImage>(_device, VK_IMAGE_TYPE_2D, extent3d, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, false, _sampleCount);
    range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    _depthImageView = std::make_unique<VulkanImageView>(_device, _depthImage.get(), VK_IMAGE_VIEW_TYPE_2D, depthFormat, mapping, range);

    // Transition depth image
    _device->ImmediateSubmit([&](VkCommandBuffer cmd){

        _depthImage->Transition(
            cmd, 
            0, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 });

        _colorImage->Transition(
            cmd,
            0, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            0, VK_ACCESS_SHADER_READ_BIT,
            { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        _colorImageResolved->Transition(
            cmd,
            0, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            0, VK_ACCESS_TRANSFER_WRITE_BIT,
            { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        _selectionImage->Transition(
            cmd, 
            0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
        
        _selectionImageResolved->Transition(
            cmd,
            0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    });

    _imagesDirty = false;

    onPostViewportResized.Broadcast(this);
}

void Viewport::SetSize(VkExtent2D extent)
{
    _extent = extent;
    _imagesDirty = true;
}

void Viewport::SetScissor(float top, float bottom, float left, float right)
{
    _scissorTop = std::clamp(top, 0.0f, 1.0f);
    _scissorBottom = std::clamp(bottom, 0.0f, 1.0f);
    _scissorLeft = std::clamp(left, 0.0f, 1.0f);
    _scissorRight = std::clamp(right, 0.0f, 1.0f);
}

void Viewport::SetProjection(const glm::mat4& projection)
{
    _uboData.projection = projection;
}

void Viewport::SetView(const glm::mat4& view)
{
    _uboData.view = view;
}

void Viewport::UpdateUniform(RenderData& rd)
{
    // Update the viewport uniform buffer.
    const VkExtent3D extent = _colorImage->GetExtent();

    _uboData.time = rd.GetCurrentFrameTime();
    _uboData.dt = rd.GetDeltaFrameTime();
    _uboData.resolution = glm::vec2 { (float)extent.width, (float)extent.height };
    _uboData.mouse = {}; // TODO: mouse position to be added later.

    _globalBuffer.Upload(std::as_bytes(std::span<ViewportUBO, 1>{&_uboData, 1}), rd.GetCurrentFrame());
}

void Viewport::PrepareForFrame(RenderData& rd)
{
}

bool Viewport::Ready()
{
    if (_imagesDirty)
    {
        RecreateImages();
        return true;
    }

    return true;
}

bool Viewport::Bind(RenderData& rd)
{
    auto cmd = rd.GetCommandBuffer();

    auto extent = GetExtent();

    if (_imagesDirty) {
        RecreateImages();
        //return false; // Skip this frame.
    }

    // Setup the vulkan viewport commands.
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);
 
    // Setup the vulkan scissor.
    // We have to convert from normalized scissor dimensions to pixels.
    _scissor.offset.x = static_cast<int32_t>(_scissorLeft * static_cast<float>(extent.width));
    _scissor.offset.y = static_cast<int32_t>(_scissorTop * static_cast<float>(extent.height));
    _scissor.extent.width = static_cast<int32_t>((1.0f - _scissorRight) * static_cast<float>(extent.width));
    _scissor.extent.height = static_cast<int32_t>((1.0f - _scissorBottom) * static_cast<float>(extent.height));
    vkCmdSetScissor(cmd, 0, 1, &_scissor);

    // Set the current sample count and descriptor set.
    rd.SetGlobalDescriptorSet(_globalDescriptorSets[rd.GetCurrentFrame()].Get());
    rd.SetSampleCount(_sampleCount);

    return true;
}

void Viewport::TransitionPreRender(RenderData& rd)
{
    auto cmd = rd.GetCommandBuffer();

    // Transition images
    if (_sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        //if (_colorImage->GetLayout() == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        //    return;

        _colorImage->Transition(
            cmd,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_ACCESS_SHADER_READ_BIT, 
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
    }
    else
    {
        _colorImageResolved->Transition(
            cmd,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
    }
}

void Viewport::TransitionPostRender(RenderData& rd)
{
    // If sampled and swapchain, transition the color image to sampled -> present.
    // If sampled and no swapchain, transition back into a color attachment.
    // If not sampled and swapchain, transition the color attachment to present.
    // If not sampled and no swapchain, do nothing?


    if (_sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        // Transition the color attachment into a sampled image
        _colorImage->Transition(
            rd.GetCommandBuffer(),
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
    }
    else
    {

        // Transition the resolve image from TRANSFER_DST, to SAMPLED_IMAGE.
        _colorImageResolved->Transition(
            rd.GetCommandBuffer(),
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // Idk how this condition would exist, what would it's purpose be?
        throw std::runtime_error("Invalid viewport pre present transition operation.");
    }
}

VkExtent2D Viewport::GetExtent() const
{
    return _extent;
}

VkImageView Viewport::GetColorImageView()
{
    return _colorImageView->Get();
}

VkImageView Viewport::GetColorResolveImageView()
{
    return _colorImageResolvedView->Get();
}

VkImageView Viewport::GetSelectionImageView()
{
    return _selectionImageView->Get();
}

VkImageView Viewport::GetSelectionResolveImageView()
{
    return _selectionImageResolvedView->Get();
}

VkImageView Viewport::GetDepthImageView()
{
    return _depthImageView->Get();
}

VkSampleCountFlagBits Viewport::GetSampleCount()
{
    return _sampleCount;
}

ViewportRenderFlags Viewport::GetRenderFlags() const
{
    return _renderFlags;
}

void Viewport::GetColorRenderingAttachments(std::vector<VkRenderingAttachmentInfo>& attachments)
{
    attachments.resize(2);

    attachments[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachments[0].pNext = nullptr;
    attachments[0].imageView = _colorImageView->Get();
    attachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].resolveMode = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_RESOLVE_MODE_NONE : VK_RESOLVE_MODE_AVERAGE_BIT;
    attachments[0].resolveImageView = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_NULL_HANDLE : _colorImageResolvedView->Get();
    attachments[0].resolveImageLayout = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_IMAGE_LAYOUT_UNDEFINED : _colorImageResolved->GetLayout();
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].clearValue = VkClearValue{VkClearColorValue{0.98f, 0.98f, 0.98f, 1.0f}};

    attachments[1].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachments[1].pNext = nullptr;
    attachments[1].imageView = _selectionImageView->Get();
    attachments[1].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[1].resolveMode = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_RESOLVE_MODE_NONE : VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
    attachments[1].resolveImageView = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_NULL_HANDLE : _selectionImageResolvedView->Get();
    attachments[1].resolveImageLayout = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_IMAGE_LAYOUT_UNDEFINED : _selectionImageResolved->GetLayout();
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].clearValue = VkClearValue {VkClearColorValue{ -1, -1, -1, -1 }};

}

void Viewport::GetDepthRenderingAttachment(VkRenderingAttachmentInfo& attachment)
{
    attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachment.pNext = nullptr;
    attachment.imageView = _depthImageView->Get();
    attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    attachment.resolveMode = VK_RESOLVE_MODE_NONE;
    attachment.resolveImageView = VK_NULL_HANDLE;
    attachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.clearValue = VkClearValue{.depthStencil = {1.0f, 0}};
}


void Viewport::FillColorRenderingAttachmentsForUI(std::vector<VkRenderingAttachmentInfo>& attachments)
{
    attachments.resize(1);

    attachments[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachments[0].pNext = nullptr;
    attachments[0].imageView = _colorImageView->Get();
    attachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].resolveMode = VK_RESOLVE_MODE_NONE;
    attachments[0].resolveImageView = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? _colorImageView->Get() : _colorImageResolvedView->Get();
    attachments[0].resolveImageLayout = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? _colorImage->GetLayout() : _colorImageResolved->GetLayout();
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].clearValue = VkClearValue{VkClearColorValue{0.98f, 0.98f, 0.98f, 1.0f}};

}

void Viewport::SetRenderFlags(ViewportRenderFlags flags)
{
    _renderFlags = flags;
}

void Viewport::QueuePresent(RenderData& rd)
{
}

void Viewport::PrepareEndFrame()
{
}

void Viewport::TransitionPrePresent(RenderData& rd)
{
}

VkImageView Viewport::GetRenderedImageView()
{
    return _sampleCount == VK_SAMPLE_COUNT_1_BIT ? _colorImageView->Get() : _colorImageResolvedView->Get();
}


}
