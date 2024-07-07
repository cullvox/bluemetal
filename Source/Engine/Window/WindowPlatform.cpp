#include "WindowPlatform.h"
#include "WindowPlatformGLFW.g"

std::span<WindowPlatformAPI> WindowPlatform::GetAvailablePlatforms() {
    static const std::vector<WindowPlatformAPI> available = {
        WindowPlatformAPI::eGLFW, 
        WindowPlatformAPI::eSDL    
    };

    return available;
}

WindowPlatformAPI WindowPlatform::GetRecommendedPlatform() {
    return GetAvailablePlatforms()[0];
}

WindowPlatformInterface* WindowPlatform::Get(WindowPlatformAPI api = GetRecommendedPlatform()) {

    // Returns a platform if one is already created.
    if (_singleton)
        return _singleton.get();

    switch (api) {
    case WindowPlatformAPI::eGLFW:
        _singleton = std::make_unique<WindowPlatformGLFW>();
        break;

    case WindowPlatformAPI::eSDL:
        _singleton = std::make_unique<WindowPlatformSDL>();
        break;
    default:
        throw std::runtime_error("Trying to use an unsupported window platform!");
    }

    return _singleton.get();
}