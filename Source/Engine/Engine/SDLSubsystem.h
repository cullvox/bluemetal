#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_vulkan.h>

namespace bl 
{

class SDLSubsystem 
{
public:
    SDLSubsystem();
    ~SDLSubsystem();
};

} /* namespace bl*/