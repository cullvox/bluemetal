#pragma once

#include "Export.h"
#include "Precompiled.h"
#include "VideoMode.h"

class BLUEMETAL_API blDisplay
{
public:
    blDisplay(int display);

    static std::vector<blDisplay> getDisplays() noexcept;

    uint32_t getIndex() const noexcept;
    const std::string& getName() const noexcept;
    const std::vector<blVideoMode>& getVideoModes() const noexcept;
    blRect2D getRect() const noexcept;
    blVideoMode getDesktopMode() const noexcept;

private:
    uint32_t                    _index;
    std::string                 _name;
    std::vector<blVideoMode>    _videoModes;
    blRect2D                    _rect;
    blVideoMode                 _desktopMode;
};