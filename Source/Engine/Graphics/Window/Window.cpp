#include "Core/Print.h"
#include "Graphics/Vulkan.h"
#include "Window.h"

namespace bl 
{

void Window::UseTemporaryWindow(std::functional<void(SDL_Window*)> func)
{
    SDL_Window temporaryWindow = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_VULKAN);

    if (!temporaryWindow) 
    {
        throw std::runtime_error("Could not create a temporary SDL window to get Vulkan instance extensions!");
    }

    func(temporaryWindow);
    SDL_DestroyWindow(temporaryWindow);
}

void Window::UseTemporarySurface(Instance* instance, const std::function<void(VkSurfaceKHR)>& func)
{
    UseTemporaryWindow([&](SDL_Window* window){
            VkSurfaceKHR temporarySurface = VK_NULL_HANDLE;
            SDL_Vulkan_CreateSurface(window, instance->Get(), &temporarySurface);
            func(temporarySurface);
            vkDestroySurfaceKHR(instance->Get(), temporarySurface, nullptr);
        });
}

std::vector<const char*> Window::GetSurfaceExtensions(Instance* instance)
{
    std::vector<const char*> extensions = {};
    UseTemporaryWindow([&](SDL_Window* window){
            unsigned int extensionsCount = 0;
            SDL_Vulkan_GetInstanceExtensions(window, &extensionsCount, nullptr);
            extensions.resize(extensionsCount);
            SDL_Vulkan_GetInstanceExtensions(window, &extensionsCount, extensions.data());
        });
    return extensions;
}

Window::Window(const VideoMode& video, const std::string& title, std::shared_ptr<Display> display)
{
    CreateWindow(video, title, display);
    CreateSurface();
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

SDL_Window* Window::Get() const 
{ 
    return _window; 
}

void Window::CreateWindow(const VideoMode& video, const std::string& title, std::shared_ptr<Display>& display)
{
    auto flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    auto x = SDL_WINDOWPOS_CENTERED;
    auto y = SDL_WINDOWPOS_CENTERED;
    auto width = video.extent.x;
    auto height = video.extent.y;

    // When a display is selected that means fullscreen is going to be enabled.
    if (display)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
        x = display->getRect().offset.x;
        y = display->getRect().offset.y;
    }

    _window = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);
    if (!_window) 
    {
        throw std::runtime_error("Could not create an SDL2 window!");
    }

    SDL_ShowWindow(_window);
}

} // namespace bl
