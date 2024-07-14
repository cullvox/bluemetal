#pragma once

#include "Math/Rect.h"
#include "Graphics/Vulkan.h"

namespace bl {

/// @brief A window object from the device's platform.
class Window {
public:

    /// @brief Destructor
    virtual ~Window() = default;

    /// @brief Gets the window rect.
    ///
    /// Gets the position and extent of the window creating a rect in display
    /// pixel space. Any backend render buffer that needs to be the size of 
    /// the framebuffer can use this function to get that data.
    ///
    /// @returns The window rect.
    ///
    virtual Rect2D GetRect() = 0;

    /// @brief Creates a Vulkan surface for this window.
    /// This surface object is not maintained by the window itself, whoever 
    /// creates this surface object is the owner of it.
    ///
    /// @param[in] instance The Vulkan instance used to create the surface.
    ///
    /// @return The new vulkan surface.
    ///
    virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;
};

} // namespace bl