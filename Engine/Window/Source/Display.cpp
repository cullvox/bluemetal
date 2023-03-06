#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"
#include "Window/Display.hpp"

#include <SDL2/SDL.h>

namespace bl {

const std::vector<VideoMode>& Display::getVideoModes() const noexcept
{
    return m_videoModes;
}

VideoMode Display::getDesktopMode() const noexcept
{
    return m_desktopMode;
}

bool Display::getDisplay(unsigned int index, Display& display)
{
    const auto displays = getDisplays();
    
    if (index >= displays.size())
    {
        Logger::Log("Out of bounds display requested!");
        return false;
    }

    display = displays[index];
    return true;
}

std::vector<Display> Display::getDisplays() noexcept
{
    // Get the number of displays and create an array for them all.
    std::vector<Display> screens{SDL_GetNumVideoDisplays()};

    for (uint32_t i = 0; i < screens.size(); i++)
    {
        Display& display = screens[i];

        // Set the displays index. 
        display.index = i;

        // Set the displays name.
        display.name = SDL_GetDisplayName(i);
        
        // Set the displays rect.
        SDL_Rect rect = {};
        SDL_GetDisplayUsableBounds(i, &rect);
        display.rect = Rect2D{rect.x, rect.y, (uint32_t)rect.w, (uint32_t)rect.h};

        // Set the video modes.
        display.m_videoModes = std::vector<VideoMode>{SDL_GetNumDisplayModes(i)};

        for (uint32_t j = 0; j < display.m_videoModes.size(); j++)
        {
            
            SDL_DisplayMode displayMode{};
            if (SDL_GetDisplayMode(i, j, &displayMode) != 0)
            {
                displayMode
                SDL_BITSPERPIXEL(displayMode.format)
                displayMode.w
            }


            const VideoMode mode{}
        }
    }

    return screens;
}

Display::Display(uint32_t index, const std::string& name, Rect2D rect, uint32_t refreshRate, const std::vector<VideoMode>& videoModes, VideoMode desktopMode)
    : index(index), name(name), rect(rect), refreshRate(refreshRate), m_videoModes(videoModes), m_desktopMode(desktopMode) { }

};