#include "Window.h"
#include "Core/Print.h"

namespace bl {

void Window::useTemporaryWindow(const std::function<void(SDL_Window*)>& func)
{
    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> temporaryWindow{
        SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_VULKAN), 
        [](SDL_Window* window){ SDL_DestroyWindow(window); }};

    if (!temporaryWindow) {
        throw std::runtime_error("Could not create a temporary SDL window to get Vulkan instance extensions!");
    }

    func(temporaryWindow.get());
}

Window::Window(
    const VideoMode&                video,
    const std::string&              title,
    std::shared_ptr<Display>        display)
{
    createWindow(video, title, display);
}

Window::~Window()
{
    SDL_DestroyWindow(_window);
}

glm::ivec2 Window::getExtent() const
{
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(_window, &width, &height);

    return glm::ivec2{width, height};
}

SDL_Window* Window::get() const { return _window; }

void Window::createWindow(
    const VideoMode&            video,
    const std::string&          title,
    std::shared_ptr<Display>&   display)
{
    // The default window flags for the engine.
    uint32_t flags = 
        SDL_WINDOW_VULKAN | 
        SDL_WINDOW_MAXIMIZED | 
        SDL_WINDOW_RESIZABLE | 
        SDL_WINDOW_ALLOW_HIGHDPI;

    // Display in fullscreen if a window was specified.
    if (display) flags |= SDL_WINDOW_FULLSCREEN;

    const int x = display ? display->getRect().offset.x : SDL_WINDOWPOS_CENTERED;
    const int y = display ? display->getRect().offset.y : SDL_WINDOWPOS_CENTERED;
    const int width = video.extent.x;
    const int height = video.extent.y;

    _window = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);

    if (!_window) {
        throw std::runtime_error("Could not create an SDL2 window!");
    }

    SDL_ShowWindow(_window);
}

} // namespace bl
