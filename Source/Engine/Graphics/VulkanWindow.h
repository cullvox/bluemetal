#pragma once

#include "Precompiled.h"
#include "Core/NonCopyable.h"
#include "Engine/SDLInitializer.h"
#include "Display.h"
#include "VideoMode.h"
#include "VulkanSwapchain.h"

namespace bl {

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;

struct VulkanWindowData final : public WindowData {
public:
    VulkanRenderWindow(VulkanDevice* device);
    ~VulkanRenderWindow();

    virtual void OnCreate();
    virtual void OnDestroy();

private:
    VulkanDevice* device;
    VkSurfaceKHR surface;
    VulkanSwapchain swapchain;

};

} // namespace bl
