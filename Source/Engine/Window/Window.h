#pragma once

#include "Math/Rect.h"
#include "Graphics/Vulkan.h"

namespace bl {

/// @brief A renderable window object from the device's window platform.
class Window {
public:

    /// @brief Destructor
    virtual ~Window() = 0;

    /// @brief Returns the windows current position and extent rect.
    virtual Rect2D GetRect() = 0;

    /// @brief Creates a Vulkan surface for this window.
    /// This surface object is not maintained by the window itself, whoever 
    /// creates this surface object is the owner of it.
    ///
    /// @param[in] instance The Vulkan instance to create the window surface.
    ///
    /// @return The new vulkan surface.
    ///
    virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;
}; 

} // namespace bl