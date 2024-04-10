#pragma once

#include "Precompiled.h"
#include "Window/Window.h"

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
    VkSurfaceKHR getSurface() const;

private:
    void createSurface();

    std::shared_ptr<GfxInstance> _instance;
    VkSurfaceKHR _surface;
};

} // namespace bl
