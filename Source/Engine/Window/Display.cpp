#include "Display.h"
#include "Core/Log.h"

blDisplay::blDisplay(int display)
{

    // get the displays rect
    SDL_Rect sdlRect{};
    if (SDL_GetDisplayUsableBounds(display, &sdlRect) < 0)
    {
        BL_LOG(blLogType::eError, "Could not get the desktop display mode for display #{}", display);
    }

    blRect2D rect(
        {(int32_t)sdlRect.x, (int32_t)sdlRect.y}, 
        {(uint32_t)sdlRect.w, (uint32_t)sdlRect.h}
    );

    // get the displays video modes
    int modeCount = SDL_GetNumDisplayModes(display);
    if (modeCount < 1)
    {
        BL_LOG(blLogType::eError, "Could not get video modes for display #{}", display);
    }

    // desktop mode
    const blVideoMode desktopMode(rect.extent, 0, 0);
    
    std::vector<blVideoMode> videoModes;
    for (int j = 0; j < modeCount; j++)
    {
        // get the display mode
        SDL_DisplayMode sdlMode = {};
        if (SDL_GetDisplayMode(display, j, &sdlMode) < 0)
        {
            BL_LOG(blLogType::eError, "Could not get a display mode #{} for display #{}!", j, display);
            continue; // skip adding this display mode because it's invalid
        }

        // get the pixel format's information
        SDL_PixelFormat* pFormat = SDL_AllocFormat(sdlMode.format); 

        // add the display mode
        videoModes.emplace_back(blExtent2D((uint32_t)sdlMode.w, (uint32_t)sdlMode.h), (uint8_t)pFormat->BitsPerPixel, (uint16_t)sdlMode.refresh_rate);

        SDL_FreeFormat(pFormat);
    }
}

uint32_t blDisplay::getIndex() const noexcept
{
    return _index;
}

const std::string& blDisplay::getName() const noexcept
{
    return _name;
}

blRect2D blDisplay::getRect() const noexcept
{
    return _rect;
}

const std::vector<blVideoMode>& blDisplay::getVideoModes() const noexcept
{
    return _videoModes;
}

blVideoMode blDisplay::getDesktopMode() const noexcept
{
    return _desktopMode;
}

std::vector<blDisplay> blDisplay::getDisplays() noexcept
{
    int displayCount = SDL_GetNumVideoDisplays();

    std::vector<blDisplay> displays;

    for (int i = 0; i < displayCount; i++)
    {
        displays.emplace_back(i);
    }

    return displays;
}