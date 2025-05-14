#include "Display.h"
#include "Core/Print.h"
#include "Engine/SDLInitializer.h"
#include <cstddef>

namespace bl 
{

Display::Display(SDL_DisplayID displayId)
    : _displayId(displayId) {}

SDL_DisplayID Display::GetDisplayID() const 
{ 
    return _displayId;
}

std::string_view Display::GetName() const
{ 
    return SDL_GetDisplayName(_displayId); 
}

Offset2D Display::GetDisplayPixelCoords() const
{
    SDL_Rect rect{};
    SDL_CHECK(SDL_GetDisplayUsableBounds(_displayId, &rect))

    return Offset2D{rect.x, rect.y};
}

Rect2D Display::GetRect()
{ 
    SDL_Rect rect{};
    SDL_CHECK(SDL_GetDisplayUsableBounds(_displayId, &rect))

    return Rect2D{{rect.x, rect.y}, {(uint32_t)rect.w, (uint32_t)rect.h}};
}

std::vector<VideoMode> Display::GetVideoModes()
{
    int sdlModeCount = 0;
    auto sdlModes = SDL_GetFullscreenDisplayModes(_displayId, &sdlModeCount);

    std::vector<VideoMode> modes{static_cast<std::size_t>(sdlModeCount)};
    for (int i = 0; i < sdlModeCount; i++) 
        modes.emplace_back(ConvertSDLDisplayMode(sdlModes[i]));

    return modes;
}

VideoMode Display::GetDesktopMode()
{
    auto mode = SDL_GetDesktopDisplayMode(_displayId);
    return ConvertSDLDisplayMode(mode);
}

VideoMode Display::ConvertSDLDisplayMode(const SDL_DisplayMode* mode)
{
    auto formatDetails = SDL_GetPixelFormatDetails(mode->format);
    return VideoMode{
        formatDetails->Rbits, 
        formatDetails->Gbits, 
        formatDetails->Bbits, 
        Extent2D{(uint32_t)mode->w, (uint32_t)mode->h}, 
        mode->refresh_rate};
}

std::vector<Display> Display::GetDisplays()
{
    int displayCount = 0;
    SDL_DisplayID* displayIds = SDL_GetDisplays(&displayCount);

    std::vector<Display> displays;
    for (int i = 0; i < displayCount; i++)
        displays.emplace_back(Display{displayIds[i]});

    return displays;
}

} // namespace bl
