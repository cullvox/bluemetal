#include "SDLInitializer.h"

namespace bl 
{

SDLInitializer::SDLInitializer()
{
    SDL_SetMainReady();

    uint32_t flags = 
        SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | 
        SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS;

    if (SDL_Init(flags) != 0) 
    {
        throw std::runtime_error("Could not initialize SDL!");
    }
}

SDLInitializer::~SDLInitializer()
{
    SDL_Quit();
}

}