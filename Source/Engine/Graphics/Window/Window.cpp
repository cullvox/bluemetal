#include "Window.h"
#include "Core/Print.h"

namespace bl {

Window::Window(const CreateInfo& createInfo)
    : _instance(createInfo.instance)
{
    createWindow(createInfo);
    createSurface();
}

Window::~Window()
{
    vkDestroySurfaceKHR(_instance->get(), _surface, nullptr);
    SDL_DestroyWindow(_pWindow);
}

Extent2D Window::getExtent() const
{
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(_pWindow, &width, &height);

    return Extent2D((uint32_t)width, (uint32_t)height);
}

VkSurfaceKHR Window::getSurface() const { return _surface; }
SDL_Window* Window::get() const { return _pWindow; }

void Window::createWindow(const CreateInfo& createInfo)
{
    // default window flags
    uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    // display in fullscreen if monitor specified
    if (createInfo.display) flags |= SDL_WINDOW_FULLSCREEN;

    const int x = createInfo.display ? createInfo.display->getRect().offset.x : SDL_WINDOWPOS_CENTERED;
    const int y = createInfo.display ? createInfo.display->getRect().offset.y : SDL_WINDOWPOS_CENTERED;
    const int width = createInfo.mode.extent.width;
    const int height = createInfo.mode.extent.height;

    _pWindow = SDL_CreateWindow(createInfo.title.c_str(), x, y, width, height, flags);

    if (!_pWindow) {
        throw std::runtime_error("Could not create an SDL2 window!");
    }

    SDL_ShowWindow(_pWindow);
}

void Window::createSurface()
{
    if (SDL_Vulkan_CreateSurface(_pWindow, _instance->get(), &_surface) != SDL_TRUE) {
        throw std::runtime_error("Could not create a Vulkan surface from an SDL window!");
    }
}

} // namespace bl
