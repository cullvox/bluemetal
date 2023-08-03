#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "Math/Vector2.h"
#include "Graphics/Instance.h"
#include "VideoMode.h"
#include "Display.h"

namespace bl
{

class BLUEMETAL_API Window
{
public:
    Window(std::shared_ptr<Instance> instance, VideoMode videoMode, const std::string& title = "Window", std::optional<Display> display = std::nullopt);
    ~Window();

    Extent2D getExtent();
    SDL_Window* getHandle();
    VkSurfaceKHR getSurface();

private:
    void createWindow(const VideoMode& videoMode, const std::string& title, std::optional<Display> display);
    void createSurface();

    SDL_Window*                 m_pWindow;
    std::shared_ptr<Instance>   m_instance;
    VkSurfaceKHR                m_surface;
};

} // namespace bl