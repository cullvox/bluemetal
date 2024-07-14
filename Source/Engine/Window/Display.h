#pragma once

#include "Precompiled.h"
#include "Math/Extent.h"
#include "Math/Rect.h"

namespace bl {

struct VideoMode {
    VideoMode(int32_t redBits, int32_t greenBits, int32_t blueBits, Extent2D extent, int refreshRate)
        : redBits(redBits)
        , greenBits(greenBits)
        , blueBits(blueBits)
        , extent(extent)
        , refreshRate(refreshRate) { }

    uint32_t redBits; ///!< 
    uint32_t greenBits;
    uint32_t blueBits;
    Extent2D extent; /// @brief Size of the display mode in pixels.
    uint32_t refreshRate; /// @brief Refresh rate of the monitor in hz. 
};

/// @brief A display object.
///
/// A monitor connected to the system.
///
class Display {
public:
    virtual ~Display() = default;

    /// @brief Gets this monitors human readable name.
    ///
    /// The system has a name for every monitor even if it's something generic
    /// it should be different enough that the user will know which one to 
    /// select.
    ///
    /// @returns A user legible name for this monitor.
    ///
    virtual std::string_view GetName() = 0;

    /// @brief Gets the virtual space offset and extent of the display.
    ///
    /// The system has a virtual screen space in pixels for every window and 
    /// display. This function is useful for placing the window inside the 
    /// monitors space and full-screen things.
    ///
    /// @returns The position and extent for the display.
    ///
    virtual Rect2D GetRect() = 0;

    /// @brief Returns the size of the display excluding the system taskbar.
    ///
    /// As describe in @ref Display::GetRect() this uses the same virtual
    /// screen space pixels. The only difference is that the returned extent
    /// excludes the taskbar and any other system considerations.
    ///
    /// @returns The position and extent of the display excluding the taskbar.
    ///
    virtual Rect2D GetWorkAreaRect() = 0;

    /// @brief Gets all video modes available on this display.
    ///
    /// All displays have a list of video modes for full screen as described
    /// by the system. Use the video modes to put a window into a full screen
    /// mode.
    ///
    /// @returns All video modes supported by this display.
    ///
    virtual std::vector<VideoMode> GetVideoModes() = 0;
};

} // namespace bl
