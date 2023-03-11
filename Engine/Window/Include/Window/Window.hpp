#pragma once

#include "Core/Export.h"
#include "Core/Precompiled.hpp"
#include "Core/Version.hpp"
#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"
#include "Window/Display.hpp"

#include <SDL2/SDL.h>

#include <vulkan/vulkan.h>

#include <string>

namespace bl
{

class BLOODLUST_API Window
{
public:

    /// @brief Default constructor.
    ///
    /// In order to open a window when using this constructor you must call
    /// open().
    /// 
    /// \sa open
    Window() noexcept = default;

    /// @brief Construct a proper window.
    /// @param videoMode Position and size of the window.
    /// @param title What the title bar displays as the name of the window.
    /// @param fullscreen If the window is in a fullscreen mode.
    Window(DisplayMode videoMode, const std::string& title = "Window", std::optional<Display> display = std::nullopt) noexcept;

    /// @brief Destruct the window if it was opened.
    ~Window() noexcept;


    /// @brief Opens a new window, will do nothing if window previously created.
    /// @param videoMode Position and size of the window.
    /// @param title What the title bar displays as the name of the window.
    /// @param fullscreen If the window is in a fullscreen mode.
    /// @return True on success, false on failure.
    [[nodiscard]] bool open(DisplayMode videoMode, const std::string& title = "Window", std::optional<Display> display = std::nullopt) noexcept;
    
    /// @brief Checks if the window construction was a success. 
    /// @return True on good window creation, false on bad window creation.
    [[nodiscard]] bool good() const noexcept;

    /// @brief Gets the size of the windows surface in pixels.
    /// @return Size in pixels of window.
    [[nodiscard]] Extent2D getExtent() const noexcept;

    /// @brief Gets the internal SDL_Window structure for other usages.
    ///         Only use if you know what you are doing so you don't screw anything up.
    /// @return SDL_Window Internal handle.
    [[nodiscard]] SDL_Window* getHandle() const noexcept;

    /// @brief Creates a VkSurfaceKHR for this window.
    ///         This window claims no ownership on the returned value, YOU are responsible for deleting it.
    /// @param instance Vulkan instance
    /// @param surface Your new surface.
    /// @return True on success, false on failure
    [[nodiscard]] bool createVulkanSurface(VkInstance instance, VkSurfaceKHR& surface) const noexcept;

    /// @brief Gets the vulkan instance surface extensions required for this window. 
    /// @param extensions The extensions to be added.
    /// @return True on success, false on failure.
    [[nodiscard]] bool getVulkanInstanceExtensions(std::vector<const char*>& extensions) const noexcept;
    
private:
    SDL_Window* m_pWindow;    
};

}; /* namespace bl */