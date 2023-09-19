#include "Display.h"
#include "Core/Print.h"

namespace bl {

Display::Display(int display)
    : _index(display)
{

    // Get the display's name.
    _name = std::string(SDL_GetDisplayName(display));

    // Get the displays rect.
    SDL_Rect sdlRect {};
    if (SDL_GetDisplayUsableBounds(display, &sdlRect) < 0) {
        blError("Could not get a rect on display #{}!", display);
        return;
    }

    _rect = Rect2D(
        Offset2D((int32_t)sdlRect.x, (int32_t)sdlRect.y), 
        Extent2D((uint32_t)sdlRect.w, (uint32_t)sdlRect.h)
    );

    // Get the displays video modes.
    int modeCount = SDL_GetNumDisplayModes(display);
    if (modeCount < 1) {
        blError("Could not get video modes for display #{}", display);
        return;
    }

    // desktop mode
    _desktopMode = VideoMode(_rect.extent, 0, 0);

    SDL_DisplayMode sdlMode = {};
    SDL_PixelFormat* pFormat = {};
    for (int modeIndex = 0; modeIndex < modeCount; modeIndex++) {
        // get the display mode
        if (SDL_GetDisplayMode(display, modeIndex, &sdlMode) < 0) {
            blWarning("Could not get display mode #{} for display #{}!", modeIndex, display);
            continue; // skip adding this display mode because it's invalid
        }

        // get the pixel format's information
        pFormat = SDL_AllocFormat(sdlMode.format);
        if (!pFormat) {
            blWarning("Could not allocate SDL Pixel Format for display #{}!", display);
            continue;
        }

        // add the display mode
        _videoModes.emplace_back(
            Extent2D((uint32_t)sdlMode.w, (uint32_t)sdlMode.h), // extent
            (uint8_t)pFormat->BitsPerPixel, // bitsPerPixel
            (uint16_t)sdlMode.refresh_rate); // refreshRate

        SDL_FreeFormat(pFormat);
    }

    blInfo("Found display: \"{}\" offset {}x{} res {}x{}", _name, _rect.offset.x, _rect.offset.y, _rect.extent.width, _rect.extent.height);
}

uint32_t Display::getIndex() const noexcept { return _index; }
const std::string& Display::getName() const noexcept { return _name; }
Rect2D Display::getRect() const noexcept { return _rect; }
const std::vector<VideoMode>& Display::getVideoModes() const noexcept { return _videoModes; }
VideoMode Display::getDesktopMode() const noexcept { return _desktopMode; }

std::vector<Display*> Display::getDisplays() noexcept
{
    int displayCount = SDL_GetNumVideoDisplays();

    static std::vector<Display*> displays = [displayCount]() {
        std::vector<Display*> temp;

        for (int i = 0; i < displayCount; i++)
            temp.push_back(new Display(i));

        return temp;
    }();

    return displays;
}

} // namespace bl
