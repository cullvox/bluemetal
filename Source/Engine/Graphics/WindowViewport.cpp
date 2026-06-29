#include "Graphics/RenderData.h"
#include "Precompiled.h"
#include "WindowViewport.h"
#include "Renderer.h"
#include "Window/Window.h"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace bl
{

WindowViewport::WindowViewport(Renderer* renderer, Window* window)
    : Viewport(renderer)
    , _renderer(renderer)
    , _window(window)
{
    if (!renderer || !window)
    {
        throw std::invalid_argument("Arguments must not be nullptr.");
    }

    if (!SDL_Vulkan_CreateSurface(window->Get(), _renderer->GetDevice()->GetInstance()->Get(), nullptr, &_surface))
    {
        throw std::runtime_error("Could not create the vulkan surface for a viewport.");
    }

    _swapchain = std::make_unique<VulkanSwapchain>(_renderer->GetDevice(), _surface);

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

WindowViewport::~WindowViewport()
{
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

    _swapchain.reset();

    vkDestroySurfaceKHR(_device->GetInstance()->Get(), _surface, nullptr);
}


Window* WindowViewport::GetWindow()
{
    return _window;
}

void WindowViewport::RecreateImages()
{        
    _swapchainImages.reserve(_swapchain->GetImageCount());
    _swapchainImageViews.reserve(_swapchain->GetImageCount());

    auto swapImages = _swapchain->GetImages();
    auto swapExtent = _swapchain->GetExtent();

    for (int i = 0; i < _swapchain->GetImageCount(); i++)
    {
        _swapchainImages.emplace_back(_device, swapImages[i], VK_IMAGE_TYPE_2D, VkExtent3D{swapExtent.width, swapExtent.height, 1}, _swapchain->GetFormat(), _swapchain->GetImageUsageFlags(), VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_UNDEFINED);
        _swapchainImageViews.emplace_back(_device, &_swapchainImages[i], VK_IMAGE_VIEW_TYPE_2D, _swapchain->GetFormat(), VkComponentMapping{VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY}, VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
    }
    // Transition all the swapchain images to color attachments.
    
    _device->ImmediateSubmit([&](VkCommandBuffer cmd) {
        for (auto& image : _swapchainImages)
        {
            image.Transition(
                cmd,
                0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
        }
    });
    
    _extent = swapExtent;

    Viewport::RecreateImages();

}

VkPresentModeKHR WindowViewport::GetPresentMode()
{
    return _swapchain->GetPresentMode();
}

bool operator==(const VkExtent2D& a, const VkExtent2D& b)
{
    return a.width == b.width && a.height == b.height;
}

bool operator!=(const VkExtent2D& a, const VkExtent2D& b)
{
    return !(a == b);
}

void WindowViewport::PrepareForFrame(RenderData& rd)
{
    // Acquire the next image from the swapchain.
    if (_swapchain->AcquireNext(_imageIndex, _imageAvailableSemaphores[rd.GetCurrentFrame()]))
    {
        _imagesDirty = true;
    }

    if (_swapchain->GetExtent().width != _extent.width)
    {
        _imagesDirty = true;
    }

    // When we're just rendering to a gpu texture there's no need for any
    // semaphores for sync, so we don't use any when it's just a render texture.

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
}

void WindowViewport::TransitionPreRender(RenderData& rd)
{
    if (_sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        // Rendering only takes place on the swapchain image during for 1x samples.
        // Swapchain must be in color attachment layout.
        _swapchainImages[_imageIndex].Transition(
            rd.GetCommandBuffer(),
            0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
    }
    else 
    {
        // For >1x samples, rendering takes place on color image, then resolved to the swapchain image.
        // Color image must be in color attachment layout.
        // Swapchain image must be in transfer dst layout.

        // From present to transfer dst.
        _swapchainImages[_imageIndex].Transition(
            rd.GetCommandBuffer(),
            0, VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            0, VK_ACCESS_TRANSFER_WRITE_BIT,
            {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // The color image should already be in color image layout.

    }

}

void WindowViewport::QueuePresent(RenderData& rd)
{
    // If we're not using a swapchain, there is nothing to present.
    if (_swapchain->QueuePresent(_imageIndex, _renderFinishedSemaphores[_imageIndex])) 
    {
        SetSize(_swapchain->GetExtent());
        RecreateImages();
    }
}

VkImageView WindowViewport::GetColorRenderImageView()
{
    if (_sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        return _swapchainImageViews[_imageIndex].Get();
    }
    else
    {
        return _colorImageView->Get();
    }
}

VkImageView WindowViewport::GetColorRenderResolveImageView()
{
    if (_sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        return VK_NULL_HANDLE;
    }
    else
    {
        return _swapchainImageViews[_imageIndex].Get();
    }
}

void WindowViewport::TransitionPostRender(RenderData& rd)
{
    // Transition to a sampled image layout.

    if (_sampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        // Transition swapchain image from COLOR_ATTACHMENT -> SAMPLED_IMAGE
        _swapchainImages[_imageIndex].Transition(
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
        // Transition swapchain image from TRANSFER_DST -> SAMPLED_IMAGE
        // Color image can stay in COLOR_ATTACHMENT

        _swapchainImages[_imageIndex].Transition(
            rd.GetCommandBuffer(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
    }

}

void WindowViewport::TransitionPrePresent(RenderData& rd)
{
    // Transition the swapchain image to a presentable layout.
    _swapchainImages[_imageIndex].Transition(
        rd.GetCommandBuffer(),
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_SHADER_READ_BIT,
        0,
        VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

}

void WindowViewport::GetColorRenderingAttachments(std::vector<VkRenderingAttachmentInfo>& attachments)
{
    attachments.resize(2);

    attachments[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachments[0].pNext = nullptr;
    attachments[0].imageView = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? _swapchainImageViews[_imageIndex].Get() : _colorImageView->Get();
    attachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].resolveMode = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_RESOLVE_MODE_NONE : VK_RESOLVE_MODE_AVERAGE_BIT;
    attachments[0].resolveImageView = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_NULL_HANDLE : _swapchainImageViews[_imageIndex].Get();
    attachments[0].resolveImageLayout = _sampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_IMAGE_LAYOUT_UNDEFINED : _swapchainImages[_imageIndex].GetLayout();
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

void WindowViewport::FillColorRenderingAttachmentsForUI(std::vector<VkRenderingAttachmentInfo>& attachments)
{
    attachments.resize(1);

    attachments[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachments[0].pNext = nullptr;
    attachments[0].imageView = _swapchainImageViews[_imageIndex].Get();
    attachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].resolveMode = VK_RESOLVE_MODE_NONE;
    attachments[0].resolveImageView = VK_NULL_HANDLE;
    attachments[0].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].clearValue = VkClearValue{VkClearColorValue{0.98f, 0.98f, 0.98f, 1.0f}};

}

VkImageView WindowViewport::GetRenderedImageView()
{
    return _swapchainImageViews[_imageIndex].Get();
}

} // namespace bl