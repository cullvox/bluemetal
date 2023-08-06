#include "Window.h"
#include "Core/Log.h"

namespace bl
{

Window::Window(
    GraphicsInstance*       pInstance, 
    VideoMode               videoMode, 
    const std::string&      title, 
    std::optional<Display>  display)
    : m_pInstance(pInstance)
{
    createWindow(videoMode, title, display);
    createSurface();
}

Window::~Window()
{
    vkDestroySurfaceKHR(m_pInstance->getHandle(), m_surface, nullptr);
    SDL_DestroyWindow(m_pWindow);
}

Extent2D Window::getExtent()
{
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(m_pWindow, &width, &height);

    return Extent2D((uint32_t)width, (uint32_t)height);
}

VkSurfaceKHR Window::getSurface()
{
    return m_surface;
}

SDL_Window* Window::getHandle()
{
    return m_pWindow;
}

void Window::createWindow(const VideoMode& videoMode, const std::string& title, std::optional<Display> display)
{
    // default window flags
    uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    // display in fullscreen if monitor specified
    if (display) flags |= SDL_WINDOW_FULLSCREEN;

    const int x = display.has_value() ? display->getRect().offset.x : SDL_WINDOWPOS_CENTERED;
    const int y = display.has_value() ? display->getRect().offset.y : SDL_WINDOWPOS_CENTERED;
    const int width = videoMode.extent.width;
    const int height = videoMode.extent.height;

    m_pWindow = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);

    if (!m_pWindow)
    {
        throw std::runtime_error("Could not create an SDL2 window! Error: " + std::string(SDL_GetError()));
    }

    SDL_ShowWindow(m_pWindow);
}

void Window::createSurface()
{
    if (SDL_Vulkan_CreateSurface(m_pWindow, m_pInstance->getHandle(), &m_surface) != SDL_TRUE)
    {
        BL_LOG(LogType::eFatal, "Could not create a Vulkan surface from an SDL window!");
    }
}

} // namespace bl