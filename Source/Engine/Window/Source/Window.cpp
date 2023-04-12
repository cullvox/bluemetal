#include "Core/Log.hpp"
#include "Window/Window.hpp"
#include "Window/Display.hpp"

#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>


blWindow::blWindow(DisplayMode videoMode, const std::string& title, std::optional<Display> display) noexcept
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error("Could not initialize SDL_INIT_VIDEO!");
    }

    // Build the flags for the window.
    uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | 
                     SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    if (display.has_value())
        flags |= SDL_WINDOW_FULLSCREEN;

    // Compute the positional values of the window.
    const int x = display.has_value() ? display->getRect().offset.x : SDL_WINDOWPOS_CENTERED;
    const int y = display.has_value() ? display->getRect().offset.y : SDL_WINDOWPOS_CENTERED;
    const int width = videoMode.extent.width;
    const int height = videoMode.extent.height;

    // Create the window.
    _pWindow = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);

    if (!_pWindow)
    {
        throw std::runtime_error("Could not create an SDL2 window! Error: " + std::string(SDL_GetError()));
    }

    SDL_ShowWindow(_pWindow);
}

blWindow::~blWindow()
{
    if (_surface)
        vkDestroySurfaceKHR()
    
    if (_pWindow) 
        SDL_DestroyWindow(_pWindow);
}

blExtent2D blWindow::getExtent() const noexcept
{
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(_pWindow, &width, &height);
    return blExtent2D{(uint32_t)width, (uint32_t)height};
}

SDL_Window* blWindow::getHandle() const noexcept
{
    return _pWindow;
}

VkSurfaceKHR blWindow::createVulkanSurface(VkInstance instance)
{
    if (SDL_Vulkan_CreateSurface(_pWindow, instance, &_surface) != SDL_TRUE)
    {
        throw std::runtime_error("Could not create a Vulkan surface from an SDL window!");
    }

    return _surface;
}

std::vector<const char*> blWindow::getVulkanInstanceExtensions() const
{
    std::vector<const char*> extensions;
    unsigned int extensionsCount = 0;
    
    if (!SDL_Vulkan_GetInstanceExtensions(_pWindow, &extensionsCount, nullptr))
    {
        throw std::runtime_error("Could not get the Vulkan instance extension count from an SDL window!");
    }

    extensions.resize(extensionsCount);
    
    if (!SDL_Vulkan_GetInstanceExtensions(_pWindow, &extensionsCount, extensions.data()))
    {
        throw std::runtime_error("Could not get the Vulkan instance extensions from an SDL window!");
    }

    return extensions;
}