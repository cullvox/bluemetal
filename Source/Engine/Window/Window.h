#pragma once

#include "Precompiled.h"
#include "Engine/SDL.h"
#include "Math/Rect.h"
#include "Display.h"

namespace bl {

/** @brief A window object from the device's platform. */
class Window
{
public:
    Window(const std::string& title, Rect2D rect, bool fullscreen);
    virtual ~Window();

    Rect2D GetRect(); /** @brief Returns the window rect in monitor space. */
    SDL_Window* Get() const; /** @brief Returns the underlying window handle. */
    Extent2D GetExtent() const; /** @brief Returns a Vulkan usable extent for swapchain. */
    VideoMode GetCurrentVideoMode() const; /** @brief Returns the current video mode. */
    void SetTitle(const std::string& title); /** @brief Changes the title displayed on the top of a windowed window. */
    void SetVideoMode(const VideoMode& mode); /** @brief Changes the windows dimensions and video mode. */

private:
    SDL_Window* _window;
};

} // namespace bl
