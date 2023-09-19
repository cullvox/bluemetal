#include "Window.h"
#include "Core/Print.h"

namespace bl {

Window::Window(const WindowCreateInfo& createInfo)
    : _pInstance(createInfo.pInstance)
{
    if (!create(createInfo))
        throw std::runtime_error("Window creation filed! Read previous logs to see what wrong.");
}

Window::~Window()
{
}

bool Window::create(const WindowCreateInfo& createInfo)
{
    return createWindow(createInfo.mode, createInfo.title, createInfo.display) && createSurface();
}

void Window::destroy()
{
    vkDestroySurfaceKHR(_pInstance->getHandle(), _surface, nullptr);
    SDL_DestroyWindow(_pWindow);
}

Extent2D Window::getExtent()
{
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(_pWindow, &width, &height);

    return Extent2D((uint32_t)width, (uint32_t)height);
}

VkSurfaceKHR Window::getSurface() { return _surface; }
SDL_Window* Window::getHandle() { return _pWindow; }

bool Window::createWindow(const VideoMode& videoMode, const std::string& title, Display* display)
{
    // default window flags
    uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    // display in fullscreen if monitor specified
    if (display) flags |= SDL_WINDOW_FULLSCREEN;

    const int x = display ? display->getRect().offset.x : SDL_WINDOWPOS_CENTERED;
    const int y = display ? display->getRect().offset.y : SDL_WINDOWPOS_CENTERED;
    const int width = videoMode.extent.width;
    const int height = videoMode.extent.height;

    _pWindow = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);

    if (!_pWindow) {
        blError("Could not create an SDL2 window! {}", SDL_GetError());
    }

    SDL_ShowWindow(_pWindow);
}

bool Window::createSurface()
{
    if (SDL_Vulkan_CreateSurface(_pWindow, _pInstance->getHandle(), &_surface) != SDL_TRUE) {
        blError("Could not create a Vulkan surface from an SDL window!");
    }
}

} // namespace bl
