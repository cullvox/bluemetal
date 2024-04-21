#include "Core/Print.h"
#include "Instance.h"
#include "RenderWindow.h"

namespace bl 
{

RenderWindow::RenderWindow(
    std::shared_ptr<GfxInstance>    instance,
    const VideoMode&                video,
    const std::string&              title,
    std::shared_ptr<Display>        display)
    : Window(video, title, display)
    , _instance(instance)
{
    createSurface();
}

RenderWindow::~RenderWindow()
{
    vkDestroySurfaceKHR(_instance->get(), _surface, nullptr);
}

VkSurfaceKHR RenderWindow::getSurface() const { return _surface; }

void RenderWindow::createSurface()
{
    if (SDL_Vulkan_CreateSurface(get(), _instance->get(), &_surface) != SDL_TRUE) {
        throw std::runtime_error("Could not create a Vulkan surface from an SDL window!");
    }
}

} // namespace bl
