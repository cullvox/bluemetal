#pragma once

#include "Precompiled.h"
#include "Math/Rect.h"
#include "Display.h"
#include "Graphics/Vulkan.h"

#include <SDL3/SDL.h>

namespace bl {

/// @brief A window object from the device's platform.
class Window
{
public:
    Window(const std::string& title, std::optional<VideoMode> videoMode, bool fullscreen);

    /// @brief Destructor
    ~Window();

    /// @brief Gets the window rect.
    ///
    /// Gets the position and extent of the window creating a rect in display
    /// pixel space. Any backend render buffer that needs to be the size of 
    /// the framebuffer can use this function to get that data.
    ///
    /// @returns The window rect.
    ///
    Rect2D GetRect();

    /// @brief Returns the underlying window handle.
    SDL_Window* Get() const;

    /// @brief Returns a Vulkan usable extent for swapchain.
    Extent2D GetExtent() const; 

    /// @brief Returns the current video mode.
    VideoMode GetCurrentVideoMode() const; 

    /// @brief Changes the title displayed on the top of a windowed window.
    void SetTitle(const std::string& title);

    /// @brief Changes the windows dimensions and video mode.
    void SetVideoMode(const VideoMode& mode);

    VkSurfaceKHR CreateSurface(VkInstance instance);

    /// @brief Returns an array of extensions the vulkan device needs.
    static std::span<const char*> GetVulkanExtensions(); 

private:
    SDL_Window* _window;
    VkSurfaceKHR surface;
};

} // namespace bl