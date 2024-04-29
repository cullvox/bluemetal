#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>

namespace bl 
{

class SDLInitializer 
{
public:
    SDLInitializer();
    ~SDLInitializer();
};

} /* namespace bl*/

#define SDL_CHECK(result) \
    if ((result) != 0) { throw std::runtime_error(SDL_GetError()); }
