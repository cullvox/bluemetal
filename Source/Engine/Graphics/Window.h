#pragma once

#include "Precompiled.h"
#include "Core/NonCopyable.h"
#include "Engine/SDLInitializer.h"
#include "Display.h"
#include "VideoMode.h"
#include "Swapchain.h"

namespace bl 
{

class Instance;
class Device;
class Swapchain;

class Window : public NonCopyable
{
public:
    Window(Device* device, const std::string& title, std::optional<VideoMode> videoMode = std::nullopt, bool fullscreen = true);
    ~Window();

    SDL_Window* Get() const; /** @brief Returns the underlying window handle. */
    VkExtent2D GetExtent() const; /** @brief Returns a Vulkan usable extent for swapchain. */
    VideoMode GetCurrentVideoMode() const; /** @brief Returns the current video mode. */
    void SetTitle(const std::string& title); /** @brief Changes the title displayed on the top of a windowed window. */
    void SetVideoMode(const VideoMode& mode); /** @brief Changes the windows dimensions and video mode. */
    VkSurfaceKHR GetSurface() const; /** @brief Returns the vulkan surface associated with this window. */
    Swapchain* GetSwapchain() const; /** @brief Returns the swapchain associated with this window. */

public:
    static void UseTemporaryWindow(const std::function<void(SDL_Window*)>& func); /** @brief Creates a temporary window that can be used for various non-display purposes. */
    static std::vector<const char*> GetSurfaceExtensions(); /** @brief Returns the surface extensions required for this device. */
    static void UseTemporarySurface(Instance* instance, const std::function<void(VkSurfaceKHR)>& func); /** @brief Creates a temporary surface for instance/device creation usage. */

private:
    Device* _device;
    SDL_Window* _window;
    VkSurfaceKHR _surface;
    std::unique_ptr<Swapchain> _swapchain;
};

} // namespace bl
