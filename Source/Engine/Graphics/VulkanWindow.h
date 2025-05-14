#pragma once

#include "Precompiled.h"
#include "Core/NonCopyable.h"
#include "Engine/SDLInitializer.h"
#include "Window/Display.h"
#include "VulkanSwapchain.h"
#include "Window/Window.h"
#include <SDL3/SDL_vulkan.h>

namespace bl {

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;

class VulkanWindow final : public Window {
public:
    VulkanWindow(VulkanDevice* device, const std::string& title, Rect2D rect, bool fullscreen);
    ~VulkanWindow();

    VkSurfaceKHR GetSurface();
    VulkanSwapchain* GetSwapchain();

private:
    VulkanDevice* _device;
    VkSurfaceKHR _surface;
    std::unique_ptr<VulkanSwapchain> _swapchain;

};

} // namespace bl
