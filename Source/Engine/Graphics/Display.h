#pragma once

#include "Export.h"
#include "Precompiled.h"
#include "Math/Rect.h"
#include "VideoMode.h"

namespace bl
{

class Display
{
public:
    std::string GetName() = 0; /** @brief Returns a human readable name of this monitor. */
    VkRect2D GetRect() = 0; /** @brief Returns the offset and extent of the display in pixels. */
    VideoMode GetDesktopMode() = 0; /** @brief Returns the main displays at windowed excluding the size of the taskbar. */
    std::vector<VideoMode> GetVideoModes() = 0; /** @brief Returns all video modes available on this display. */
    uint32_t GetID() = 0; /** @brief Returns the a unique index for this display, used for config settings. */

};

} // namespace bl
