#pragma once

#include "Export.h"
#include "Precompiled.h"
#include "Math/Vector2.h"
#include "VideoMode.h"

class BLUEMETAL_API blDisplay 
{
public:
    static std::vector<blDisplay> getDisplays() noexcept;

    uint32_t getIndex() const noexcept;
    const std::string& getName() const noexcept;
    const std::vector<blVideoMode>& getVideoModes() const noexcept;
    blRect2D getRect() const noexcept;
    blVideoMode getDesktopMode() const noexcept;

private:
    blDisplay() = default;
    blDisplay(uint32_t index, const std::string& name, const std::vector<blVideoMode>& videoMode, blRect2D rect, blVideoMode desktopMode);

    uint32_t                    _index;
    std::string                 _name;
    std::vector<blVideoMode>    _videoModes;
    blRect2D                    _rect;
    blVideoMode                 _desktopMode;
};