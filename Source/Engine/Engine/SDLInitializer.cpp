#include "SDLInitializer.h"

#include "Precompiled.h"
#include <SDL3/SDL_init.h>

namespace bl 
{

SDLInitializer::SDLInitializer()
{

    uint32_t flags = 
        SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | 
        SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS;

    if (SDL_Init(flags) == false) 
    {
        throw std::runtime_error("Could not initialize SDL!");
    }
}

SDLInitializer::~SDLInitializer()
{
    SDL_Quit();
}

}