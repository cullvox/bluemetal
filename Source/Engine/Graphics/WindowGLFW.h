#pragma once

#include <SDL2/SDL.h>

namespace bl {

class WindowSDL {
public:
    WindowSDL();
    ~WindowSDL();

private:
    SDL_Window* _window;
};

}