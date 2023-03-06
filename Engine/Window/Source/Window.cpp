#include "Core/Precompiled.hpp"
#include "Core/Log.hpp"
#include "Window/Window.hpp"
#include "Window/Display.hpp"

#include <SDL2/SDL_vulkan.h>

namespace bl {

Window::Window(VideoMode videoMode, const std::string& title, bool fullscreen) noexcept
{
    if (!open(videoMode, title, fullscreen))
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

bool Window::open(VideoMode videoMode, const std::string& title, bool fullscreen) noexcept
{

    // Build the flags for the window.
    uint32_t flags = SDL_WINDOW_VULKAN;
    
    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    // Create the window.
    m_pWindow = SDL_CreateWindow(title.c_str(), videoMode.rect.offset.x, videoMode.rect.offset.y, videoMode.rect.extent.width, videoMode.rect.extent.height, flags);
    
    if (!m_pWindow)
    {
        Logger::Log("Could not create an SDL2 window!");
        return false;
    }

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
    SDL_GetWindowSize(m_pWindow, &width, &height);
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