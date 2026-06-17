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

Viewport::Viewport(VulkanDevice* device, VkExtent2D extent)
    : _device(device)
    , _swapchain(nullptr)
    , _imageAvailableSemaphores({})
    , _renderFinishedSemaphores({})
    , _sampleCount(VK_SAMPLE_COUNT_1_BIT)
    , _extent(extent)
{

    // Create the viewport descriptor sets to bind later.
    std::vector<VkDescriptorSetLayoutBinding> bindings { 1 };
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    _globalDescriptorSetLayout = _device->AcquireDescriptorSetLayout(bindings);
    _globalBuffer = VulkanBufferFrameRing{ _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(bl::GlobalUBO) };

    std::array<VkDescriptorBufferInfo, VulkanConfig::maxFramesInFlight> descriptorBufferInfos;
    std::array<VkWriteDescriptorSet, VulkanConfig::maxFramesInFlight> descriptorWrites;

    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _globalDescriptorSets[i] = _device->AllocateDescriptorSet(_globalDescriptorSetLayout);

        descriptorBufferInfos[i] = _globalBuffer.GetDescriptorInfo(i);

        descriptorWrites[i] = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = _globalDescriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &descriptorBufferInfos[i],
        };
    }

    vkUpdateDescriptorSets(_device->Get(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

    // Create the viewport images.
    RecreateImages();

    // Create the ImGui context for this viewport.
    //_context = ImGui::CreateContext();

}

Viewport::Viewport(VulkanDevice* device, VulkanSwapchain* swapchain)
    : Viewport(device, swapchain->GetExtent())
{
    _swapchain = swapchain;
    
    // Build out the per-frame semaphores infos.
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;

    // Create the semaphores for each frame in flight.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) 
    {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]))
    }

    // Create a render-finished semaphore for each frame in the synchronization driver.
    uint32_t imageCount = _swapchain->GetImageCount();
    _renderFinishedSemaphores.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; i++)
    {
        VK_CHECK(vkCreateSemaphore(_device->Get(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]))
    }

    RecreateImages();
}

Viewport::~Viewport()
{
    // Free the viewport global descriptor sets.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _device->FreeDescriptorSet(_globalDescriptorSets[i], _globalDescriptorSetLayout);
    }

    // If not using a swapchain, no semaphores are used and do not need to be freed.
    if (!_swapchain)
    {
        return;
    }

    // Destroy all per-frame in flight semaphores.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        vkDestroySemaphore(_device->Get(), _imageAvailableSemaphores[i], nullptr);
    }

    // Destroy all per-image render finished semaphores. 
    for (uint32_t i = 0; i < _swapchain->GetImageCount(); i++)
    {
        vkDestroySemaphore(_device->Get(), _renderFinishedSemaphores[i], nullptr);
    }
}

void Viewport::RecreateImages()
{
    // Create color image
    VkComponentMapping mapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    auto colorFormat = _device->GetPhysicalDevice()->FindSupportedFormat({VK_FORMAT_R16G16B16A16_SFLOAT, VK_FORMAT_R8G8B8A8_UNORM}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_2_SAMPLED_IMAGE_BIT);
    auto depthFormat = _device->GetPhysicalDevice()->FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    auto selectionFormat = _device->GetPhysicalDevice()->FindSupportedFormat({VK_FORMAT_R32_UINT}, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);

    VkExtent3D extent3d = { _extent.width, _extent.height, 1 };

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

    if (_swapchain) {
        
        _swapchainImages.reserve(_swapchain->GetImageCount());
        _swapchainImageViews.reserve(_swapchain->GetImageCount());

        auto swapImages = _swapchain->GetImages();
        auto swapExtent = _swapchain->GetExtent();

        for (int i = 0; i < _swapchain->GetImageCount(); i++)
        {
            _swapchainImages[i] = VulkanImage{_device, swapImages[i], VK_IMAGE_TYPE_2D, VkExtent3D{swapExtent.width, swapExtent.height, 1}, _swapchain->GetFormat(), _swapchain->GetImageUsageFlags(), VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_UNDEFINED};
            _swapchainImageViews[i] = VulkanImageView(_device, &_swapchainImages[i], VK_IMAGE_VIEW_TYPE_2D, _swapchain->GetFormat(), VkComponentMapping{VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY}, VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        }
    }

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
            0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

        _colorImageResolved->Transition(
            cmd,
            0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            0, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
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

        if (_swapchain)
        {
            // Transition all the swapchain images to color attachments.
            for (auto& image : _swapchainImages)
            {
                image.Transition(
                    cmd, 
                    0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
            }
        }
    });



    _imagesDirty = false;
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
    _projection = projection;
}

void Viewport::SetView(const glm::mat4& view)
{
    _view = view;
}

void Viewport::UpdateUniform(RenderData& rd)
{
    // Update the viewport uniform buffer.
    const VkExtent3D extent = _colorImage->GetExtent();

    GlobalUBO uboData = {};
    uboData.time = rd.GetCurrentFrameTime();
    uboData.dt = rd.GetDeltaFrameTime();
    uboData.resolution = glm::vec2 { (float)extent.width, (float)extent.height };
    uboData.mouse = {}; // TODO: mouse position to be added later.
    uboData.projection = _projection;
    uboData.view = _view;

    _globalBuffer.Upload(std::as_bytes(std::span<GlobalUBO, 1>{&uboData, 1}), rd.GetCurrentFrame());
}

bool operator==(const VkExtent2D& a, const VkExtent2D& b)
{
    return a.width == b.width && a.height == b.height;
}

bool operator!=(const VkExtent2D& a, const VkExtent2D& b)
{
    return !(a == b);
}

bool Viewport::Bind(RenderData& rd)
{
    auto cmd = rd.GetCommandBuffer();

    auto extent = GetExtent();

    if (_swapchain) 
    {
        if (_swapchain->AcquireNext(_imageIndex, _imageAvailableSemaphores[rd.GetCurrentFrame()]))
        {
            _imagesDirty = true;
        }
    }

    if (_swapchain && _swapchain->GetExtent() != _extent)
    {
        _imagesDirty = true;
    }

    if (_imagesDirty) {
        RecreateImages();
        return false; // Skip this frame.
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
    _scissor.offset.x = static_cast<int32_t>((1.0f - _scissorLeft) * static_cast<float>(extent.width));
    _scissor.offset.y = static_cast<int32_t>((1.0f - _scissorTop) * static_cast<float>(extent.height));
    _scissor.extent.width = static_cast<int32_t>(_scissorRight * static_cast<float>(extent.width));
    _scissor.extent.height = static_cast<int32_t>(_scissorBottom * static_cast<float>(extent.height));
    vkCmdSetScissor(cmd, 0, 1, &_scissor);

    // Set the current sample count and descriptor set.
    rd.SetGlobalDescriptorSet(_globalDescriptorSets[rd.GetCurrentFrame()]);
    rd.SetSampleCount(_sampleCount);

    // When we're just rendering to a gpu texture there's no need for any
    // semaphores for sync, so we don't use any when it's just a render texture.
    if (!_swapchain) 
    {
        return true;
    }

    // Add the swapchain image's wait semaphore and render finished semaphore.
    VkSemaphoreSubmitInfo waitInfo = {};
    waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    waitInfo.pNext = nullptr;
    waitInfo.semaphore = _imageAvailableSemaphores[rd.GetCurrentFrame()];
    waitInfo.value = 0;
    waitInfo.stageMask = 0;
    waitInfo.deviceIndex = 0;
    rd.AddRenderWaitSemaphore(waitInfo);

    waitInfo.semaphore = _renderFinishedSemaphores[_imageIndex];
    waitInfo.value = 0;
    waitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    waitInfo.deviceIndex = 0;
    rd.AddRenderSignalSemaphore(waitInfo);

    return true;
}

void Viewport::TransitionPostRender(RenderData& rd)
{

    // Only transition the color image if it's going to be sampled later.
    if (!HasFlag(_renderFlags, ViewportRenderFlags::eSampled))
    {
        return;   
    }

    if (_sampleCount == VK_SAMPLE_COUNT_1_BIT) {

        if (_swapchain) {
            _swapchainImages[rd.GetImageIndex()].Transition(
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
            _colorImage->Transition(
                rd.GetCommandBuffer(), 
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT,
                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        }
    }
    else 
    {

        if (_swapchain) {
            _swapchainImages[rd.GetImageIndex()].Transition(
                rd.GetCommandBuffer(), 
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT,
                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        } else {
            _colorImageResolved->Transition(
                rd.GetCommandBuffer(), 
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT,
                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        }
    }
}

void Viewport::TransitionPrePresent(RenderData& rd)
{
    // If sampled and swapchain, transition the color image to sampled -> present
    // If sampled and no swapchain, transition back into a color attachment.
    // If not sampled and swapchain, transition the color attachment to present.
    // If not sampled and no swapchain, do nothing?

    if (_swapchain)
    {
        if (HasFlag(_renderFlags, ViewportRenderFlags::eSampled)) {

            // This image was probably sampled, so it's sampled -> present.
            _swapchainImages[rd.GetImageIndex()].Transition(
                rd.GetCommandBuffer(), 
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_ACCESS_SHADER_READ_BIT,
                0,
                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        }
        else
        {
            // Image is not being sampled, so it's color attachment -> present.
            _swapchainImages[rd.GetImageIndex()].Transition(
                rd.GetCommandBuffer(), 
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                0,
                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        }
    }
    else 
    {
        // No swapchain.

        if (HasFlag(_renderFlags, ViewportRenderFlags::eSampled))
        {
            // Has no swapchain, and was sampled, so it's sampled -> color attachment.
            _colorImage->Transition(
                rd.GetCommandBuffer(),
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                VK_ACCESS_SHADER_READ_BIT,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        }
        else
        {
            // Idk how this condition would exist, what would it's purpose be?
            throw std::runtime_error("Invalid viewport pre present transition operation.");
        }
    
    }
}

void Viewport::QueuePresent(RenderData& rd)
{
    // If we're not using a swapchain, there is nothing to present.
    if (!_swapchain) 
    {
        return;
    }

    if (_swapchain->QueuePresent(_imageIndex, _renderFinishedSemaphores[_imageIndex])) 
    {
        SetSize(_swapchain->GetExtent());
        RecreateImages();
    }
}

VkExtent2D Viewport::GetExtent() const
{
    return _extent;
}

VkImageView Viewport::GetColorImageView()
{
    if (_swapchain && _sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        return _swapchainImageViews[_imageIndex].Get();
    }
    else if (_swapchain && _sampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        return _colorImageResolvedView->Get();
    }
    else if (!_swapchain && _sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        return _colorImageView->Get();
    }
    else if (!_swapchain && _sampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        return _colorImageView->Get();
    }
}

VkImageView Viewport::GetColorResolveImageView()
{
    if (_swapchain && _sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        return _swapchainImageViews[_imageIndex].Get();
    }
    else if (_swapchain && _sampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        return _colorImageResolvedView->Get();
    }
    else if (!_swapchain && _sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        return _colorImageView->Get();
    }
    else if (!_swapchain && _sampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        return _colorImageResolvedView->Get();
    }
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

VkPresentModeKHR Viewport::GetPresentMode()
{
    if (!_swapchain) return VK_PRESENT_MODE_FIFO_KHR;
    return _swapchain->GetPresentMode();
}

ViewportRenderFlags Viewport::GetRenderFlags() const
{
    return _renderFlags;
}

void Viewport::SetRenderFlags(ViewportRenderFlags flags)
{
    _renderFlags = flags;
}

}
