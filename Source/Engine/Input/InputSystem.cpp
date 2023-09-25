#include "Input/InputSystem.hpp"
#include "Core/Log.hpp"
#include "Input/InputConversions.hpp"

blInputSystem::blInputSystem() { }

blInputSystem::~blInputSystem() noexcept { }

std::shared_ptr<blInputSystem> blInputSystem::getInstance() noexcept
{
    static std::shared_ptr<blInputSystem> instance = std::make_shared<blInputSystem>();
    return instance;
}

void blInputSystem::processKeyboard() const noexcept { }

void blInputSystem::poll() noexcept
{
    SDL_PumpEvents();

    processKeyboard();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        _hookCallbacks(&event);
        // for (blInputObserver* observer : _observers)
        // observer->onEvent(&event);

        switch (event.type) {
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                _shouldClose = true;
            }
            break;
        }
    }
}

blButton blInputSystem::getKey(blKey key) const noexcept
{
    int keystatesCount = 0;
    const uint8_t* pKeystates = SDL_GetKeyboardState(&keystatesCount);

    (void)key;
    (void)pKeystates;

    // pKeystates
    return blButton::eReleased;
}

bool blInputSystem::shouldClose() const noexcept { return _shouldClose; }

blInputHookCallback blInputSystem::addHook(const blInputHookCallbackFunction& function)
{
    return _hookCallbacks.add(function);
}

void blInputSystem::removeHook(blInputHookCallback callback) { _hookCallbacks.remove(callback); }