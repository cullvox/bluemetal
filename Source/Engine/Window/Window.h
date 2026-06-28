#pragma once


#include "Display.h"
#include "Engine/SDL.h"
#include "Math/Rect.h"

namespace bl {

/** @brief A window object from the device's platform. */
class Window {
    friend class InputSystem;
    SDL_Window* _window;
    bool _closeRequested { false };
    bool _minimized { false };
    bool _focused { false };

protected:
    friend class InputSystem;
    void RequestClose(bool close);
    void SetMinimized(bool minimized);
    void SetFocused(bool focused);

public:
    Window(const std::string& title, Rect2D rect, bool fullscreen);
    virtual ~Window();

    Rect2D GetRect(); /** @brief Returns the window rect in monitor space. */
    SDL_Window* Get() const; /** @brief Returns the underlying window handle. */
    VkExtent2D GetExtent() const; /** @brief Returns a Vulkan usable extent for swapchain. */
    VideoMode GetCurrentVideoMode() const; /** @brief Returns the current video mode. */
    void SetTitle(const std::string& title); /** @brief Changes the title displayed on the top of a windowed window. */
    void SetVideoMode(const VideoMode& mode); /** @brief Changes the windows dimensions and video mode. */
    void RequestClose(); /** @brief Requests the window to close. */

    // Input
    bool GetCloseRequested() const;
    bool GetMinimized() const;
    bool GetFocused() const;
};

} // namespace bl
