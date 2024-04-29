#include "Core/Print.h"
#include "Graphics/Vulkan.h"
#include "Graphics/Instance.h"
#include "Window.h"

namespace bl 
{

void Window::UseTemporaryWindow(const std::function<void(SDL_Window*)>& func)
{
    SDL_Window* temporaryWindow = SDL_CreateWindow("", 0, 0, 1, 1, SDL_WINDOW_VULKAN);

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

Window::Window(Device* device, const std::string& title, std::optional<VideoMode> mode)
{
    CreateWindow(video, title, display);
    CreateSurface();
}

Window::~Window()
{
    SDL_DestroyWindow(_window);
}

VkExtent2D Window::GetExtent() const
{
    int w = 0, h = 0;
    SDL_Vulkan_GetDrawableSize(_window, &w, &h);
    return VkExtent2D{(uint32_t)w, (uint32_t)h};
}

SDL_Window* Window::Get() const 
{ 
    return _window; 
}

void Window::CreateWindow(Device* device, const std::string& title, std::optional<VideoMode> videoMode, bool fullscreen)
{
    auto flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    
    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    
    auto x = SDL_WINDOWPOS_CENTERED;
    auto y = SDL_WINDOWPOS_CENTERED;
    auto w = 1080;
    auto h = 720;

    if (videoMode)
    {
        auto mode = videoMode.value();
        x = mode.rect.offset.x;
        y = mode.rect.offset.y;
        w = mode.rect.extent.width;
        h = mode.rect.extent.height;
    }

    _window = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
    if (!_window) 
    {
        throw std::runtime_error("Could not create an SDL2 window!");
    }

    SDL_ShowWindow(_window);
}

} // namespace bl
