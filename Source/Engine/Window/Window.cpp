#include "Window.h"
#include <vulkan/vulkan_core.h>

blWindow::blWindow(std::shared_ptr<blInstance> instance, const std::string& title, blVideoMode videoMode, std::optional<blDisplay> display)
{
    createWindow(videoMode, title, display);
    createSurface();
}

blWindow::~blWindow()
{
    vkDestroySurfaceKHR(_instance->getInstance(), _surface, nullptr);
    SDL_DestroyWindow(_pWindow);
}

blExtent2D blWindow::getExtent()
{
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(_pWindow, &width, &height);

    return blExtent2D((uint32_t)width, (uint32_t)height);
}

SDL_Window* blWindow::getHandle()
{
    return _pWindow;
}

void blWindow::createWindow(const blVideoMode& videoMode, const std::string& title, std::optional<blDisplay> display)
{
    // default window flags
    uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    // display in fullscreen if monitor specified
    if (display) flags |= SDL_WINDOW_FULLSCREEN;

    const int x = display.has_value() ? display->getRect().offset.x : SDL_WINDOWPOS_CENTERED;
    const int y = display.has_value() ? display->getRect().offset.y : SDL_WINDOWPOS_CENTERED;
    const int width = videoMode.extent.width;
    const int height = videoMode.extent.height;

    _pWindow = SDL_CreateWindow(title.c_str(), x, y, width, height, flags);

    if (!_pWindow)
    {
        throw std::runtime_error("Could not create an SDL2 window! Error: " + std::string(SDL_GetError()));
    }

    SDL_ShowWindow(_pWindow);
}

void blWindow::createSurface()
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (SDL_Vulkan_CreateSurface(_pWindow, _instance->getInstance(), &surface) != SDL_TRUE)
    {
        BL_LOG(blLogType::eFatal, "Could not create a Vulkan surface from an SDL window!");
    }

    return surface;
}

std::vector<const char*> blWindow::getVulkanInstanceExtensions() const
{

}

