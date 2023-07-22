#pragma once

#include "Export.h"
#include "Precompiled.h"
#include "VideoMode.h"

namespace bl
{

class BLUEMETAL_API Display
{
public:
    Display(int display);

    static std::vector<Display> getDisplays() noexcept;

    uint32_t getIndex() const noexcept;
    const std::string& getName() const noexcept;
    const std::vector<VideoMode>& getVideoModes() const noexcept;
    Rect2D getRect() const noexcept;
    VideoMode getDesktopMode() const noexcept;

private:
    uint32_t                _index;
    std::string             _name;
    std::vector<VideoMode>  _videoModes;
    Rect2D                  _rect;
    VideoMode               _desktopMode;
};

} // namespace bl