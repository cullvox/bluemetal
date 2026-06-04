#pragma once

#include <memory>
#include <string>
#include <span>

#include "VulkanSwapchain.h"
#include "Window/Window.h"
#include "VulkanViewport.h"

namespace bl {

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;

class VulkanWindow : public Window, public VulkanViewport {
    VulkanDevice* _device;
    VkSurfaceKHR _surface;
    std::unique_ptr<VulkanSwapchain> _swapchain;

public:
    VulkanWindow(VulkanDevice* device, const std::string& title, Rect2D rect, bool fullscreen);
    ~VulkanWindow();

    static std::span<const char*> GetVulkanExtensions(); /** @brief Returns an array of extensions the vulkan device needs. */
    VulkanDevice* GetDevice() const;
    VkSurfaceKHR GetSurface();
    VulkanSwapchain* GetSwapchain();
};

} // namespace bl
