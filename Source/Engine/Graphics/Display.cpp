#include "Display.h"
#include "Core/Print.h"
#include "Engine/SDLInitializer.h"

namespace bl 
{

Display::Display(int display)
    : _display(display)
{
}

Display::~Display() = default;

int Display::Get() const 
{ 
    return _display;
}

std::string Display::GetName() const 
{ 
    return SDL_GetDisplayName(_display); 
}

VkOffset2D Display::GetOffset() const
{
    SDL_Rect rect{};
    SDL_CHECK(SDL_GetDisplayUsableBounds(_display, &rect))

    return VkOffset2D{rect.x, rect.y};
}

VkRect2D Display::GetRect() const 
{ 
    SDL_Rect rect{};
    SDL_CHECK(SDL_GetDisplayUsableBounds(_display, &rect))
    
    return VkRect2D{{rect.x, rect.y}, {(uint32_t)rect.w, (uint32_t)rect.h}};
}

std::vector<VideoMode> Display::GetVideoModes() const 
{ 
    auto num = SDL_GetNumDisplayModes(_display);
    auto offset = GetOffset();

    std::vector<VideoMode> modes{};
    SDL_DisplayMode mode{};
    for (int i = 0; i < num; i++) 
    {
        SDL_CHECK(SDL_GetDisplayMode(_display, i, &mode))
        modes.emplace_back(VkRect2D{offset, {(uint32_t)mode.w, (uint32_t)mode.h}}, mode.refresh_rate);
    }

    return modes;
}

VideoMode Display::GetDesktopMode() const 
{
    SDL_DisplayMode mode{};
    SDL_CHECK(SDL_GetDesktopDisplayMode(_display, &mode))

    return VideoMode{{GetOffset(), {(uint32_t)mode.w, (uint32_t)mode.h}}, mode.refresh_rate};
}

} // namespace bl
