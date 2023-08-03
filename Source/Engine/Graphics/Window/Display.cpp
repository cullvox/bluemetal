#include "Display.h"
#include "Core/Log.h"

namespace bl
{

Display::Display(int display)
    : m_index(display)
{

    // Get the display's name
    m_name = std::string(SDL_GetDisplayName(display));

    // get the displays rect
    SDL_Rect sdlRect{};
    if (SDL_GetDisplayUsableBounds(display, &sdlRect) < 0)
    {
        BL_LOG(LogType::eError, "Could not get the desktop display mode for display #{}", display);
    }

    m_rect = Rect2D(
        {(int32_t)sdlRect.x, (int32_t)sdlRect.y},
        {(uint32_t)sdlRect.w, (uint32_t)sdlRect.h}
    );

    // get the displays video modes
    int modeCount = SDL_GetNumDisplayModes(display);
    if (modeCount < 1)
    {
        BL_LOG(LogType::eError, "Could not get video modes for display #{}", display);
    }

    // desktop mode
    m_desktopMode = VideoMode(m_rect.extent, 0, 0);
    
    for (int j = 0; j < modeCount; j++)
    {
        // get the display mode
        SDL_DisplayMode sdlMode = {};
        if (SDL_GetDisplayMode(display, j, &sdlMode) < 0)
        {
            BL_LOG(LogType::eError, "Could not get a display mode #{} for display #{}!", j, display);
            continue; // skip adding this display mode because it's invalid
        }

        // get the pixel format's information
        SDL_PixelFormat* pFormat = SDL_AllocFormat(sdlMode.format); 

        // add the display mode
        m_videoModes.emplace_back(Extent2D((uint32_t)sdlMode.w, (uint32_t)sdlMode.h), (uint8_t)pFormat->BitsPerPixel, (uint16_t)sdlMode.refresh_rate);

        SDL_FreeFormat(pFormat);
    }
}

uint32_t Display::getIndex() const noexcept
{
    return m_index;
}

const std::string& Display::getName() const noexcept
{
    return m_name;
}

Rect2D Display::getRect() const noexcept
{
    return m_rect;
}

const std::vector<VideoMode>& Display::getVideoModes() const noexcept
{
    return m_videoModes;
}

VideoMode Display::getDesktopMode() const noexcept
{
    return m_desktopMode;
}

std::vector<Display> Display::getDisplays() noexcept
{
    int displayCount = SDL_GetNumVideoDisplays();

    std::vector<Display> displays = {};

    for (int i = 0; i < displayCount; i++)
    {
        displays.emplace_back(i);
    }

    return displays;
}

Display::Display(const Display& rhs)
    : m_index(rhs.m_index)
    , m_name(rhs.m_name)
    , m_videoModes(rhs.m_videoModes)
    , m_rect(rhs.m_rect)
    , m_desktopMode(rhs.m_desktopMode)
{
}

} // namespace bl