#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "VideoMode.h"
#include "Display.h"
#include "Math/Vector2.h"
#include "Render/Instance.h"
#include <memory>
#include <vulkan/vulkan_core.h>

class BLUEMETAL_API blWindow
{
public:
    explicit blWindow(std::shared_ptr<blInstance> instance, blVideoMode videoMode, const std::string& title = "Window", std::optional<blDisplay> display = std::nullopt);
    ~blWindow();

    blExtent2D getExtent();
    SDL_Window* getHandle();
    VkSurfaceKHR getSurface();

private:
    void createWindow(const blVideoMode& videoMode, const std::string& title, std::optional<blDisplay> display);
    void createSurface();

    SDL_Window*                 _pWindow;
    std::shared_ptr<blInstance> _instance;
    VkSurfaceKHR                _surface;
};