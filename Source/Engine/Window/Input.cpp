#include "Input.h"

namespace bl {

InputSystem::InputSystem(Engine& engine)
    : System(engine)
{
}

std::shared_ptr<Resource> InputSystem::ConstructResource(ResourceSystem*, std::size_t, const std::filesystem::path&)
{
    throw std::runtime_error("Input system does not construct any resources!");
}

void InputSystem::Poll(std::function<void(SDL_Event&)> extraFunc)
{
    SDL_Event event {};
    while (SDL_PollEvent(&event)) {
        SDL_PropertiesID props = SDL_GetWindowProperties(SDL_GetWindowFromEvent(&event));
        Window* window = static_cast<Window*>(SDL_GetPointerProperty(props, "user", nullptr));

        switch (event.type) {
        case SDL_EVENT_WINDOW_MINIMIZED:
            if (window)
                window->SetMinimized(true);
            break;
        case SDL_EVENT_WINDOW_RESTORED:
            if (window)
                window->SetMinimized(false);
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            if (window)
                window->SetFocused(true);
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            if (window)
                window->SetFocused(false);
            break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (window)
                window->SetClose(true);
            break;
        // case SDL_EVENT_MOUSE_MOTION:
        //     _mouse.SetLocation({event.motion.x, event.motion.y});
        //     _mouse.SetRelativeMotion({event.motion.xrel, event.motion.yrel});
        //     break;
        //  case SDL_EVENT_MOUSE_BUTTON_DOWN:
        //      _mouse.SetMouseButtonFlag(event.button.button, true);
        //      break;
        //  case SDL_EVENT_MOUSE_BUTTON_UP:
        //      _mouse.SetMouseButtonFlag(event.button.button, false);
        //      break;
        case SDL_EVENT_MOUSE_WHEEL:
            _mouse.SetWheel({ event.wheel.x, event.wheel.y });
            break;
        }

        extraFunc(event);
    }

    SDL_PumpEvents();
    _keyboard.Poll();
    _mouse.Poll();
}

Keyboard& InputSystem::GetKeyboard()
{
    return _keyboard;
}

Mouse& InputSystem::GetMouse()
{
    return _mouse;
}

}
