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
    uint32_t GetIndex() const; /** @brief Returns the index of this window, it's unique so usable in configs. */
    const std::string& GetName() const; /** @brief Returns a human readable name of this monitor. */
    Rect2i GetRect() const; /** @brief Returns the offset and extent of the display in pixels. */  
    VideoMode GetDesktopMode() const; /** @brief Returns the main displays at windowed excluding the size of the taskbar. */
    const std::vector<VideoMode>& GetVideoModes() const; /** @brief Returns all video modes available on this display. */
    static std::vector<Display*> GetDisplays(); /** @brief Returns all displays on the system. */

private:
    Display(int display);
    
    uint32_t _index;
    std::string _name;
    Rect2i _rect;
    VideoMode _desktopMode;
    std::vector<VideoMode> _videoModes;
};

} // namespace bl
