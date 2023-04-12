#include "Core/Log.hpp"
#include "Input/InputSystem.hpp"
#include "Input/InputConversions.hpp"


blInputSystem::blInputSystem()
{
}

blInputSystem::~blInputSystem() noexcept
{
}

void blInputSystem::processKeyboard() const noexcept
{
    
}

void blInputSystem::poll() noexcept
{
    SDL_PumpEvents();

    processKeyboard();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        //for (blInputObserver* observer : _observers)
            // observer->onEvent(&event);
    }
}

blButton blInputSystem::getKey(blKey key) const noexcept
{
    int keystatesCount = 0;
    const uint8_t* pKeystates = SDL_GetKeyboardState(&keystatesCount);

    //pKeystates
    return blButton::eReleased;
}