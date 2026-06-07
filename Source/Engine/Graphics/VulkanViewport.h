#pragma once

#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanImageView.h"
#include <vulkan/vulkan_core.h>

namespace bl {

class VulkanDevice;
class VulkanImage;
class VulkanImageView;
class Renderer;
class VulkanRenderImage;
class RenderData;

// Viewports manage their own frames and render images. They are responsible for presenting to the swapchain, but not for managing synchronization or determining what gets drawn.
class VulkanViewport {
public:
    virtual ~VulkanViewport() = 0;

    virtual VkExtent2D GetExtent() = 0;
    virtual VulkanImage* GetImage() = 0;
    virtual VulkanImageView* GetImageView() = 0;
    virtual bool IsActive() = 0; // When a viewport is deactivated, the renderer will not render to it or change it in any way.
    virtual bool IsSynchronized() = 0;

    // SYNCHRONIZATION ONLY FUNCTIONS (IsSynchronized() must be true)
    virtual bool AcquireNextImage(VkSemaphore imageAvailableSemaphore);
    virtual uint32_t GetSynchronizedImageCount() = 0;
    virtual void Present(VkSemaphore renderFinishedSemaphore) = 0;
    virtual uint32_t GetCurrentImageIndex();
};

} // namespace bl
