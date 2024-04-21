#pragma once

#include "Window/Window.h"
#include "Swapchain.h"

namespace bl {

class GfxInstance;

class RenderWindow : public Window
{
public:
    RenderWindow(
        std::shared_ptr<GfxInstance>    instance,
        const VideoMode&                video,
        const std::string&              title,
        std::shared_ptr<Display>        display = nullptr);
    ~RenderWindow();

public:
    /** @brief Returns the Vulkan surface created with this window. */
    VkSurfaceKHR GetSurface() const;

private:
    void CreateSurface();
    void CreateSwapchain();

    Instance& _instance;
    VkSurfaceKHR _surface;
    Swapchain _swapchain;
};

} // namespace bl
