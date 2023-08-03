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
    Display(const Display& rhs);

    static std::vector<Display> getDisplays() noexcept;

    uint32_t getIndex() const noexcept;
    const std::string& getName() const noexcept;
    const std::vector<VideoMode>& getVideoModes() const noexcept;
    Rect2D getRect() const noexcept;
    VideoMode getDesktopMode() const noexcept;

private:
    

    uint32_t                m_index;
    std::string             m_name;
    std::vector<VideoMode>  m_videoModes;
    Rect2D                  m_rect;
    VideoMode               m_desktopMode;
};

} // namespace bl