#pragma once

#include "Core/Precompiled.hpp"
#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"

namespace bl 
{

class Display 
{
public:

    /// @brief Deleted default constructor, displays are only to be created by this class.
    Display() = delete;

    /// @brief Default destructor
    ~Display() noexcept = default;


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
    [[nodiscard]] const std::vector<VideoMode>& getVideoModes() const noexcept;

    /// @brief Gets video mode for the main display, no fullscreen.
    /// @return The video mode.
    [[nodiscard]] VideoMode getDesktopMode() const noexcept;

    /// @brief Gets all screens on the display.
    /// @param index The index of the specified display.  
    /// @return True on success, false on failure.
    [[nodiscard]] static Display getDisplay(unsigned int index);

    /// @brief Gets all the screens currently on the computer.
    /// @return The displays.
    [[nodiscard]] static std::vector<Display> getDisplays() noexcept;

private:

    /// @brief A private constructor for screen, they can only be created by this class.
    /// @param index The index of this monitor on the system.
    /// @param name The name of the monitor given by the system.
    /// @param rect The position and size of the monitor.
    /// @param videoModes All video modes of the display.
    /// @param desktopMode The desktop mode of the display.
    Display(uint32_t index, const std::string& name, Rect2D rect, const std::vector<VideoMode>& videoModes, VideoMode desktopMode);

    /// @brief The index of this monitor on the system.
    uint32_t index;

    /// @brief The name of the monitor given by the system.
    std::string name;

    /// @brief The position and size of the monitor.
    Rect2D rect;

    /// @brief All video modes of the display.
    std::vector<VideoMode> m_videoModes;

    /// @brief The desktop mode of the display.
    VideoMode m_desktopMode;

};

} /* namespace bl */