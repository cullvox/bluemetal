#include "SDLSubsystem.h"
#include "Precompiled.h"

namespace bl 
{

SDLSubsystem::SDLSubsystem()
{
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        throw std::runtime_error("Could not initialize SDL!");
    }
}

SDLSubsystem::~SDLSubsystem()
{
    SDL_Quit();
}

}