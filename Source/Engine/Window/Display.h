#pragma once

#include "Precompiled.h"
#include "Math/Extent.h"
#include "Math/Rect.h"
#include "VideoMode.h"

namespace bl {


struct VideoMode  {
    VideoMode(int32_t redBits, int32_t greenBits, int32_t blueBits, Rect2D rect, int refreshRate)
        : redBits(redBits), greenBits(greenBits), blueBits(blueBits), rect(rect), refreshRate(refreshRate) {}

    int32_t redBits; ///!< 
    int32_t greenBits;
    int32_t blueBits;
    Rect2D rect; /// @brief Size of the display mode in pixels.
    int refreshRate; /// @brief Refresh rate of the monitor in hz. 
};

/// @brief 
class Display {
public:
    virtual ~Display() = default;

    /// @brief Returns a human readable name of this monitor.
    virtual std::string_view GetName() = 0;

    /// @brief Returns the screen space offset of the pixel start of the display.
    virtual Offset2D GetOffset() = 0;

    /// @brief Returns the screen space pixel extent of the display.
    virtual Extent2D GetExtent() = 0;

    /// @brief Returns the screen space offset and extent of the display in pixels.
    virtual Rect2D GetRect() = 0;

    /// @brief Returns the screen space rect excluding the size of the OS taskbar and or menus.
    virtual Rect2D GetWorkareaRect() = 0;

    /// @brief Returns all video modes available on this display.
    virtual std::vector<VideoMode> GetVideoModes() = 0;
};

} // namespace bl
