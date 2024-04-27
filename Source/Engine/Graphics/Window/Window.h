#pragma once

#include "Precompiled.h"
#include "Core/NonCopyable.h"
#include "Engine/SDLSubsystem.h"
#include "Display.h"
#include "VideoMode.h"

namespace bl 
{

class Device;
class Swapchain;

class Window : public NonCopyable
{
public:
    Window(Device* device);
    ~Window();

    VideoMode GetCurrentVideoMode() const; /** @brief Returns the current video mode. */
    void SetTitle(const std::string& title); /** @brief Changes the title displayed on the top of a windowed window. */
    void SetVideoMode(const VideoMode& mode);

public:
    static void UseTemporaryWindow(const std::function<void(SDL_Window*)>& func); /** @brief Creates a temporary window that can be used for various non-display purposes. */
    static void UseTemporarySurface(Instance* instance, const std::function<void(VkSurfaceKHR)>& func); /** @brief Creates a temporary surface for instance/device creation usage. */
    static std::vector<const char*> GetSurfaceExtensions(Instance* instance); /** @brief Returns the surface extensions required for this device. */

private:
    SDL_Window* _window;
    VkSurfaceKHR _surface;
    std::unique_ptr<Swapchain> _swapchain;
};

} // namespace bl
