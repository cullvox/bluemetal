#include "Window.h"
#include "Core/Print.h"

#include <SDL3/SDL_vulkan.h>
#include <cstddef>

namespace bl {

Window::Window(const std::string& title, Rect2D rect, bool fullscreen)
    : _window(nullptr)
    , _closeRequested(false)
{
    auto flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

    _window = SDL_CreateWindow(title.c_str(), rect.extent.width, rect.extent.height, flags);
    if (!_window)
        throw std::runtime_error("Could not create an SDL window!");

    SDL_PropertiesID props = SDL_GetWindowProperties(_window);
    SDL_SetPointerProperty(props, "user", this);
    SDL_ShowWindow(_window);
}

Window::~Window()
{
    SDL_DestroyWindow(_window);
}

void Window::RequestClose()
{
    _closeRequested = true;
}

void Window::SetMinimized(bool minimized)
{
    _minimized = minimized;
}

void Window::SetFocused(bool focused)
{
    _focused = focused;
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

    return VideoMode { details->Rbits, details->Gbits, details->Bbits, { (uint32_t)mode->w, (uint32_t)mode->h }, mode->refresh_rate };
}

SDL_Window* Window::Get() const
{
    return _window;
}

Extent2D Window::GetExtent() const
{
    int w = 0, h = 0;
    SDL_GetWindowSizeInPixels(_window, &w, &h);

    return Extent2D { (uint32_t)w, (uint32_t)h };
}

bool Window::GetCloseRequested() const
{
    return _closeRequested;
}

bool Window::GetMinimized() const
{
    return _minimized;
}

bool Window::GetFocused() const
{
    return _focused;
}

} // namespace bl
