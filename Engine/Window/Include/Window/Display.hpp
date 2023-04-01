#pragma once

#include "Core/Precompiled.hpp"
#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"
#include "Generated/Export_Window.h"

namespace bl 
{

class BLOODLUST_WINDOW_API Display 
{
public:

    /// @brief Default destructor
    ~Display() noexcept;

    /// @brief Copy constructor
    Display(const Display& display);

    /// @brief Gets the index of this display under the settings of the computer.
    /// @return The index of the display.
    [[nodiscard]] uint32_t getIndex() const noexcept;

    /// @brief Gets the name of the display.
    /// @return The displays name.
    [[nodiscard]] const std::string& getName() const noexcept;

    /// @brief Gets the rect where and how large this monitor is in pixels.
    /// @return The rect of the monitor.
    [[nodiscard]] Rect2D getRect() const noexcept;

    /// @brief Gets every available video mode of this monitor.
    /// @return The video modes.
    [[nodiscard]] const std::vector<DisplayMode>& getVideoModes() const noexcept;

    /// @brief Gets video mode for the main display, no Fullscreen.
    /// @return The video mode.
    [[nodiscard]] DisplayMode getDesktopMode() const noexcept;

    /// @brief Gets all the screens currently on the computer.
    /// @return The displays.
    [[nodiscard]] static std::vector<Display> getDisplays() noexcept;

private:

    /// @brief Private default constructor
    Display() noexcept;

    /// @brief The index of this monitor on the system.
    uint32_t m_index;

    /// @brief The name of the monitor given by the system.
    std::string m_name;

    /// @brief The position and size of the monitor.
    Rect2D m_rect;

    /// @brief All video modes of the display.
    std::vector<DisplayMode> m_videoModes{};

    /// @brief The desktop mode of the display.
    DisplayMode m_desktopMode{};

};

} /* namespace bl */