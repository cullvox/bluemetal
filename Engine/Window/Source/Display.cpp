#include "Core/Log.hpp"
#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"
#include "Window/Display.hpp"

#include <SDL2/SDL.h>

namespace bl
{

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

const std::vector<DisplayMode> &Display::getVideoModes() const noexcept
{
    return m_videoModes;
}

DisplayMode Display::getDesktopMode() const noexcept
{
    return m_desktopMode;
}

std::vector<Display> Display::getDisplays() noexcept
{
    // Get the number of displays and create an array for them all.
    int displayCount = SDL_GetNumVideoDisplays();

    std::vector<Display> displays{};
    for (uint32_t displayIndex = 0; displayIndex < displayCount; displayIndex++)
    {
        Display display{};

        // Set the displays index.
        display.m_index = displayIndex;

        // Set the displays name.
        display.m_name = SDL_GetDisplayName(displayIndex);

        // Set the displays rect.
        SDL_Rect rect = {};
        SDL_GetDisplayUsableBounds(displayIndex, &rect);

        display.m_rect = {
            rect.x, 
            rect.y, 
            (uint32_t)rect.w, 
            (uint32_t)rect.h
        };

        // Get the amount of display modes.
        int displayModeCount = SDL_GetNumDisplayModes(displayIndex);
        if (displayModeCount < 1)
        {
            Logger::Error("Could not get any display modes for display #{}: {}", display.getIndex(), display.getName());
            continue; // Skip adding this display because it has no modes.
        }

        // Get the desktop display mode
        SDL_DisplayMode rawDisplayMode{};
        if (SDL_GetDesktopDisplayMode(displayIndex, &rawDisplayMode) < 0)
        {
            Logger::Error("Could not get the desktop display mode for display #{}: {}", display.getIndex(), display.getName());
            continue; // Skip adding this display.
        }

        // Create the bl::DisplayMode using the SDL_DisplayMode data.
        const DisplayMode desktopMode{
            {
                (uint32_t)rawDisplayMode.w, 
                (uint32_t)rawDisplayMode.h,
            },
            SDL_BITSPERPIXEL(rawDisplayMode.format),
            (uint16_t)rawDisplayMode.refresh_rate
        };

        // Set the desktop mode.
        display.m_desktopMode = desktopMode;
        
        // Iterate through all the SDL_DisplayMode's and create our own bl::DisplayMode.
        std::vector<DisplayMode> displayModes{};
        
        for (uint32_t displayModeIndex = 0; displayModeIndex < displayModeCount; displayModeIndex++)
        {

            // Get the SDL_DisplayMode of this index.
            if (SDL_GetDisplayMode(displayIndex, displayModeIndex, &rawDisplayMode) < 0)
            {
                Logger::Error("Could not get a display mode #{} for display #{}: {}", displayModeIndex, display.getIndex(), display.getName());
                continue; // Skip adding this display mode because it's invalid.
            }

            // Create our bl::DisplayMode from the SDL_DisplayMode.
            const DisplayMode mode{
                {
                    (uint32_t)rawDisplayMode.w, 
                    (uint32_t)rawDisplayMode.h
                },
                SDL_BITSPERPIXEL(rawDisplayMode.format),
                (uint16_t)rawDisplayMode.refresh_rate,
            };

            // Add the display mode.
            displayModes.emplace_back(mode);
        }

        // Set the display modes.
        display.m_videoModes = displayModes;

        // Add the display to the list.
        displays.emplace_back(display);
    }

    return displays;
}

} // namespace bl
