//===========================//
#include "WindowPlatform.hpp"
#include "render/window/WindowPlatformGLFW.hpp"
//===========================//

std::shared_ptr<IWindowPlatform> CreateWindowPlatform(EWindowPlatformAPI api)
{
    switch(api)
    {
        case EWindowPlatformAPI::eGLFW: return std::make_shared<CWindowPlatformGLFW>();
        default: throw std::runtime_error("Invalid window platform api!");
    }
}