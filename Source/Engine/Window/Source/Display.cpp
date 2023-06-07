#include "Core/Log.hpp"
#include "Window/VideoMode.hpp"
#include "Window/Display.hpp"

#include <SDL.h>
#include <SDL_pixels.h>

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

const std::vector<blDisplayMode>& blDisplay::getVideoModes() const noexcept
{
    return _videoModes;
}

blDisplayMode blDisplay::getDesktopMode() const noexcept
{
    return _desktopMode;
}

std::vector<blDisplay> blDisplay::getDisplays() noexcept
{
    int displayCount = SDL_GetNumVideoDisplays();

    std::vector<blDisplay> displays{};
    for (int displayIndex = 0; displayIndex < displayCount; displayIndex++)
    {
        blDisplay display{};

        display._index = displayIndex;
        display._name = SDL_GetDisplayName(displayIndex);

        // Set the displays maximum rect
        SDL_Rect rect{};
        if (SDL_GetDisplayUsableBounds(displayIndex, &rect) < 0)
        {
            BL_LOG(blLogType::eError, "Could not get the desktop display mode for display #{}: {}\n", display.getIndex(), display.getName());
            continue;
        }

        display._rect = 
        {
            rect.x, 
            rect.y, 
            (uint32_t)rect.w, 
            (uint32_t)rect.h
        };

        int displayModeCount = SDL_GetNumDisplayModes(displayIndex);
        if (displayModeCount < 1)
        {
            BL_LOG(blLogType::eError, "Could not get any display modes for display #{}: {}\n", display.getIndex(), display.getName());
            continue;
        }

        const blDisplayMode desktopMode
        {
            { (uint32_t)rect.w, (uint32_t)rect.h, },
            0,
            0
        };

        display._desktopMode = desktopMode;
        
        // Set all the displays desktop modes
        std::vector<blDisplayMode> displayModes{};
        SDL_DisplayMode rawDisplayMode{};
        for (int displayModeIndex = 0; displayModeIndex < displayModeCount; displayModeIndex++)
        {

            // Get the SDL_DisplayMode of this index.
            if (SDL_GetDisplayMode(displayIndex, displayModeIndex, &rawDisplayMode) < 0)
            {
                BL_LOG(blLogType::eError, "Could not get a display mode #{} for display #{}: {}\n", displayModeIndex, display.getIndex(), display.getName());
                continue; // Skip adding this display mode because it's invalid.
            }

            // Get the pixel format's information
            SDL_PixelFormat* pFormat = SDL_AllocFormat(rawDisplayMode.format); 

            // Create our bl::DisplayMode from the SDL_DisplayMode.
            const blDisplayMode mode
            {
                { (uint32_t)rawDisplayMode.w,  (uint32_t)rawDisplayMode.h },
                pFormat->BitsPerPixel,
                (uint16_t)rawDisplayMode.refresh_rate,
            };

            SDL_FreeFormat(pFormat);

            // Add the display mode.
            displayModes.emplace_back(mode);
        }

        display._videoModes = displayModes;
        displays.emplace_back(display);
    }

    return displays;
}