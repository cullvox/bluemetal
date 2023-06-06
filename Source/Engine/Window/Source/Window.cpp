#include "Core/Log.hpp"
#include "Window/Window.hpp"
#include "Window/Display.hpp"

#include <vulkan/vulkan.h>
#include <SDL_vulkan.h>


blWindow::blWindow(const blDisplayMode& videoMode, const std::string& title, std::optional<blDisplay> display)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error("Could not initialize SDL_INIT_VIDEO!");
    }

    // Build the flags for the window.
    uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | 
                     SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    if (display)
        flags |= SDL_WINDOW_FULLSCREEN;

    // Compute the positional values of the window.
    const int x = display.has_value() ? display->getRect().offset.width : SDL_WINDOWPOS_CENTERED;
    const int y = display.has_value() ? display->getRect().offset.height : SDL_WINDOWPOS_CENTERED;
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
    if (_pWindow) SDL_DestroyWindow(_pWindow);
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

vk::SurfaceKHR blWindow::createSurface(std::shared_ptr<const blWindow> window,
                                            vk::Instance instance)
{
    VkSurfaceKHR tempSurface = VK_NULL_HANDLE;
    if (SDL_Vulkan_CreateSurface(window->getHandle(), instance, &tempSurface) != SDL_TRUE)
    {
        BL_LOG(blLogType::eFatal,  "Could not create a Vulkan surface from an SDL window!");
    }

    return (vk::SurfaceKHR)tempSurface;
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