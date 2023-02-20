#pragma once

//===========================//
#include <vulkan/vulkan.h>
//===========================//

/* Notice: You can create a window using the IWindowPlatform interface. */

class IWindow
{
public:
    static inline const int DEFAULT_WIDTH = 1080;
    static inline const int DEFAULT_HEIGHT = 720;
    static inline const int DEFAULT_FULLSCREEN = 1;
    IWindow() = default;
    ~IWindow() = default;
    virtual VkSurfaceKHR CreateSurface(VkInstance instance) = 0;
    virtual VkExtent2D GetExtent() const noexcept = 0;
};