#pragma once

#include "Core/Precompiled.hpp"
#include "Core/Version.hpp"
#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"
#include "Window/Display.hpp"
#include "Window/Export.h"

#include <SDL.h>

#include <vulkan/vulkan.hpp>

class BLUEMETAL_WINDOW_API blWindow
{
public:
    blWindow(const blDisplayMode& videoMode, 
        const std::string& title = "Window", 
        std::optional<blDisplay> display = std::nullopt);
    ~blWindow() noexcept;

    blExtent2D getExtent() const noexcept;
    SDL_Window* getHandle() const noexcept;
    std::vector<const char*> getVulkanInstanceExtensions() const;

    static vk::SurfaceKHR createSurface(std::shared_ptr<const blWindow> window,
                                            vk::Instance instance);

private:
    SDL_Window* _pWindow;
};