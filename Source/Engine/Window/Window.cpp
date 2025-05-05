#include "Core/Print.h"
#include "Window.h"

#include <SDL3/SDL_vulkan.h>
#include <cstddef>

namespace bl  {

Window::Window(const std::string& title, std::optional<VideoMode> videoMode, bool fullscreen)
{
    auto flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    auto w = 1080;
    auto h = 720;

    if (videoMode) 
    {
        auto mode = videoMode.value();
        w = mode.extent.width;
        h = mode.extent.height;
    }

    _window = SDL_CreateWindow(title.c_str(), w, h, flags);
    if (!_window) 
    {
        throw std::runtime_error("Could not create an SDL window!");
    }

    SDL_ShowWindow(_window);
}

Window::~Window() 
{
    SDL_DestroyWindow(_window);
}

VideoMode Window::GetCurrentVideoMode() const 
{
    int x = 0, y = 0;
    SDL_GetWindowPosition(_window, &x, &y);

    auto mode = SDL_GetWindowFullscreenMode(_window);
    if (!mode)
        throw std::runtime_error("Could not get a window fullscreen mode!");

    auto details = SDL_GetPixelFormatDetails(mode->format);
    if (!details)
        throw std::runtime_error("Could not get pixel format details!");

    return VideoMode{details->Rbits, details->Gbits, details->Bbits, {(uint32_t)mode->w, (uint32_t)mode->h}, mode->refresh_rate};
}

SDL_Window* Window::Get() const 
{ 
    return _window; 
}

Extent2D Window::GetExtent() const
{
    int w = 0, h = 0;
    SDL_GetWindowSizeInPixels(_window, &w, &h);

    return Extent2D{(uint32_t)w, (uint32_t)h};
}

VkSurfaceKHR Window::CreateSurface(VkInstance instance)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(_window, instance, nullptr, &surface))
        throw std::runtime_error("Could not create a Vulkan surface!");

    return surface;
}

std::span<const char*> Window::GetVulkanExtensions() {

    static uint32_t extensionCount = 0;
    static const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    static std::vector<const char*> ext{extensions, extensions + extensionCount};

    return std::span<const char*>{ext.begin(), ext.end()};
}

} // namespace bl
