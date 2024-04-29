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
    Display(int display);
    ~Display();

    int Get() const; /** @brief Returns the a unique index for this display, used for config settings. */
    std::string GetName() const; /** @brief Returns a human readable name of this monitor. */
    VkOffset2D GetOffset() const; /** @brief Returns the offset of the pixel start of the display. */
    VkExtent2D GetExtent() const; /** @brief Returns the pixel size of the display. */
    VkRect2D GetRect() const; /** @brief Returns the offset and extent of the display in pixels. */
    VideoMode GetDesktopMode() const; /** @brief Returns the main displays at windowed excluding the size of the taskbar. */
    std::vector<VideoMode> GetVideoModes() const; /** @brief Returns all video modes available on this display. */

private:
    int _display;
};

} // namespace bl
