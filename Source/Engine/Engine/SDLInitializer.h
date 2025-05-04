#pragma once

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

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
    if ((result) == false) { throw std::runtime_error(SDL_GetError()); }
