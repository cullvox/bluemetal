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

    virtual void SetExtent(VkExtent2D extent) = 0;
    virtual VkExtent2D GetExtent() = 0;
    VulkanImage* GetImage() = 0;
    VulkanImageView* GetImageView() = 0;
};

} // namespace bl
