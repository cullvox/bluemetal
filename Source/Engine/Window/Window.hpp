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
    blWindow(const blDisplayMode& videoMode, const std::string& title = "Window", std::optional<blDisplay> display = std::nullopt);
    blWindow(blWindow&& other);
    ~blWindow() noexcept;

    blExtent2D getExtent() const noexcept;
    SDL_Window* getHandle() const noexcept;
    std::vector<const char*> getVulkanInstanceExtensions() const;
    vk::UniqueSurfaceKHR createSurface(vk::Instance instance) const;

private:
    SDL_Window* _pWindow;
};