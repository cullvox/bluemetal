#include "Core/Print.h"
#include "Vulkan.h"
#include "Device.h"
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

std::vector<const char*> Window::GetSurfaceExtensions()
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

Window::Window(Device* device, const std::string& title, std::optional<VideoMode> videoMode, bool fullscreen)
    : _device(device)
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
        throw std::runtime_error("Could not create an SDL window!");
    }

    SDL_ShowWindow(_window);

    if (SDL_Vulkan_CreateSurface(_window, _device->GetInstance()->Get(), &_surface) != SDL_TRUE)
    {
        throw std::runtime_error("Could not create an Vulkan surface!");
    }

    _swapchain = std::make_unique<Swapchain>(_device, this);
}

Window::~Window()
{
    _swapchain.reset();
    vkDestroySurfaceKHR(_device->GetInstance()->Get(), _surface, nullptr);
    SDL_DestroyWindow(_window);
}

VideoMode Window::GetCurrentVideoMode() const
{
    int x = 0, y = 0;
    SDL_GetWindowPosition(_window, &x, &y);

    SDL_DisplayMode mode{};
    SDL_GetWindowDisplayMode(_window, &mode);

    return VideoMode{{{x, y}, {(uint32_t)mode.w, (uint32_t)mode.h}}, mode.refresh_rate};
}

SDL_Window* Window::Get() const 
{ 
    return _window; 
}

VkSurfaceKHR Window::GetSurface() const
{
    return _surface;
}

Swapchain* Window::GetSwapchain() const
{
    return _swapchain.get();
}

VkExtent2D Window::GetExtent() const
{
    int w = 0, h = 0;
    SDL_Vulkan_GetDrawableSize(_window, &w, &h);

    return VkExtent2D{(uint32_t)w, (uint32_t)h};
}

} // namespace bl
