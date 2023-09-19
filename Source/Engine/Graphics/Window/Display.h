#pragma once

#include "Export.h"
#include "Precompiled.h"
#include "VideoMode.h"

namespace bl
{

class BLUEMETAL_API Display
{
public:
    /** @brief Returns the index of this window, it's unique so usable in configs. */
    uint32_t getIndex() const noexcept;

    /** @brief Returns a human readable name of this monitor. */
    const std::string& getName() const noexcept;

    /** @brief Returns the offset and extent of the display in pixels. */  
    Rect2D getRect() const noexcept;

    /** @brief Returns the main displays at windowed excluding the size of the taskbar. */
    VideoMode getDesktopMode() const noexcept;

    /** @brief Returns all video modes available on this display. */
    const std::vector<VideoMode>& getVideoModes() const noexcept;

    /** @brief Returns all displays on the system. */
    static std::vector<Display*> getDisplays() noexcept;

private:
    Display(int display);
    
    uint32_t                _index;
    std::string             _name;
    Rect2D                  _rect;
    VideoMode               _desktopMode;
    std::vector<VideoMode>  _videoModes;
};

} // namespace bl
