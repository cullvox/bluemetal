#pragma once

#include "Engine/SDL.h"
#include "Precompiled.h"
#include "VulkanDeviceObject.h"
#include "VulkanSwapchain.h"
#include "Window/Display.h"
#include "Window/Window.h"

namespace bl {

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;

class VulkanWindow final : public VulkanDeviceObject, public Window {
    VkSurfaceKHR _surface;
    std::unique_ptr<VulkanSwapchain> _swapchain;

public:
    VulkanWindow(VulkanDevice* device, const std::string& title, Rect2D rect, bool fullscreen);
    ~VulkanWindow();

    static std::span<const char*> GetVulkanExtensions(); /** @brief Returns an array of extensions the vulkan device needs. */
    VkSurfaceKHR GetSurface();
    VulkanSwapchain* GetSwapchain();
};

} // namespace bl
