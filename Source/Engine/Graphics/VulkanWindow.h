#pragma once

#include <memory>
#include <string>
#include <span>

#include "VulkanSwapchain.h"
#include "Window/Window.h"
#include "Viewport.h"

namespace bl {

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;

class VulkanWindow : public Window {
    VulkanDevice* _device;
    VkSurfaceKHR _surface;
    std::unique_ptr<VulkanSwapchain> _swapchain;
    std::unique_ptr<Viewport> _viewport;

public:
    VulkanWindow(VulkanDevice* device, const std::string& title, Rect2D rect, bool fullscreen);
    ~VulkanWindow();

    static std::span<const char*> GetVulkanExtensions(); /** @brief Returns an array of extensions the vulkan device needs. */
    VulkanDevice* GetDevice() const;
    VkSurfaceKHR GetSurface();
    VulkanSwapchain* GetSwapchain();
    Viewport* GetViewport();
};

} // namespace bl
