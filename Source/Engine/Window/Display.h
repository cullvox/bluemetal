#pragma once

#include "Export.h"
#include "Precompiled.h"
#include "Math/Rect.h"
#include "VideoMode.h"

namespace bl
{

class BLUEMETAL_API Display
{
public:
    /** @brief Returns the index of this window, it's unique so usable in configs. */
    uint32_t getIndex() const;

    /** @brief Returns a human readable name of this monitor. */
    const std::string& getName() const;

    /** @brief Returns the offset and extent of the display in pixels. */  
    Rect2i getRect() const;

    /** @brief Returns the main displays at windowed excluding the size of the taskbar. */
    VideoMode getDesktopMode() const;

    /** @brief Returns all video modes available on this display. */
    const std::vector<VideoMode>& getVideoModes() const;

    /** @brief Returns all displays on the system. */
    static std::vector<Display*> getDisplays();

private:
    Display(int display);
    
    uint32_t                _index;
    std::string             _name;
    Rect2i                  _rect;
    VideoMode               _desktopMode;
    std::vector<VideoMode>  _videoModes;
};

} // namespace bl
