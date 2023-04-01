#include "Core/Precompiled.hpp"
#include "Core/Log.hpp"
#include "Window/Window.hpp"
#include "Window/Display.hpp"

#include <SDL2/SDL_vulkan.h>

namespace bl {

Window::Window(DisplayMode videoMode, const std::string& title, std::optional<Display> display) noexcept
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        Logger::Error("Could not construct window, SDL_Init(SDL_INIT_VIDEO) failed!");
    }

    if (!open(videoMode, title, display))
    {
        Logger::Log("Could not properly construct a window!");
    }
}

Window::~Window()
{
    if (m_pWindow)
    {
        SDL_DestroyWindow(m_pWindow);
    }
}

Window& Window::operator=(Window&& rhs) noexcept
{
    m_pWindow = rhs.m_pWindow;
    rhs.m_pWindow = nullptr;
    return *this;
}

bool Window::open(DisplayMode videoMode, const std::string& title, std::optional<Display> display) noexcept
{

    // Build the flags for the window.
    uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    
    if (display.has_value())
        flags |= SDL_WINDOW_FULLSCREEN;

    // Compute the positional values of the window.
    const int x = display.has_value() ? display->getRect().offset.x : SDL_WINDOWPOS_CENTERED;
    const int y = display.has_value() ? display->getRect().offset.y : SDL_WINDOWPOS_CENTERED;
    const int width = videoMode.extent.width;
    const int height = videoMode.extent.height;

    // Create the window.
    m_pWindow = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);
    
    if (!m_pWindow)
    {
        Logger::Log("Could not create an SDL2 window! {}", SDL_GetError());
        return false;
    }

    SDL_ShowWindow(m_pWindow);

    return true;
}

bool Window::good() const noexcept
{
    // A window is considered bad if it was not created.
    return m_pWindow != nullptr;
}

Extent2D Window::getExtent() const noexcept
{
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(m_pWindow, &width, &height);
    return Extent2D{(uint32_t)width, (uint32_t)height};
}

SDL_Window* Window::getHandle() const noexcept
{
    return m_pWindow;
}

bool Window::createVulkanSurface(VkInstance instance, VkSurfaceKHR& surface) const noexcept
{
    return SDL_Vulkan_CreateSurface(m_pWindow, instance, &surface);
}

bool Window::getVulkanInstanceExtensions(std::vector<const char*>& extensions) const noexcept
{
    unsigned int extensionsCount = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(m_pWindow, &extensionsCount, nullptr))
    {
        Logger::Log("Could not get the vulkan instance extension count!");
    }

    extensions.resize(extensionsCount);
    
    if (!SDL_Vulkan_GetInstanceExtensions(m_pWindow, &extensionsCount, extensions.data()))
    {
        Logger::Log("Could not get the vulkan instance extensions!");
    }

    return true;
}

} /* namespace bl */