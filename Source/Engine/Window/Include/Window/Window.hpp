#pragma once

#include "Core/Precompiled.hpp"
#include "Core/Version.hpp"
#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"
#include "Window/Display.hpp"
#include "Window/Export.h"

#include <SDL2/SDL.h>

#include <vulkan/vulkan.h>

class BLOODLUST_WINDOW_API blWindow
{
public:
    blWindow(const blDisplayMode& videoMode, 
        const std::string_view& title = "Window", 
        std::optional<blDisplay> display = std::nullopt);
    ~blWindow() noexcept;

    VkSurfaceKHR createVulkanSurface(VkInstance instance);
    VkSurfaceKHR getVulkanSurface() const noexcept;
    blExtent2D getExtent() const noexcept;
    SDL_Window* getHandle() const noexcept;
    std::vector<const char*> getVulkanInstanceExtensions() const;

private:
    SDL_Window* _pWindow;
    VkSurfaceKHR _surface;
};