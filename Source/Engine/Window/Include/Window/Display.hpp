#pragma once

#include "Core/Precompiled.hpp"
#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"
#include "Window/Export.h"

class BLUEMETAL_WINDOW_API blDisplay 
{
public:
    static std::vector<blDisplay> getDisplays() noexcept;

    uint32_t getIndex() const noexcept;
    const std::string& getName() const noexcept;
    const std::vector<blDisplayMode>& getVideoModes() const noexcept;
    blRect2D getRect() const noexcept;
    blDisplayMode getDesktopMode() const noexcept;

private:
    blDisplay() noexcept {};

    uint32_t                    _index;
    std::string                 _name;
    std::vector<blDisplayMode>  _videoModes;
    blRect2D                    _rect;
    blDisplayMode               _desktopMode;

};