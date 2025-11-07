#include "Mouse.h"

namespace bl
{

// MouseButton Mouse::MouseButtonFromSDL(int buttonFlag)
// {
//     switch (buttonFlag)
//     {
//         // case SDL_BUTTON_LEFT: MouseButton
//     }
// }

void Mouse::Poll()
{
    _buttonFlags = SDL_GetMouseState(&_location.x, &_location.y);
    _relative = _location - _lastLocation;
    _lastLocation = _location;
}

int Mouse::MouseButtonToSDL(MouseButton button)
{
    switch (button)
    {
    case MouseButton::Left: return SDL_BUTTON_LEFT;
    case MouseButton::Middle: return SDL_BUTTON_MIDDLE;
    case MouseButton::Right: return SDL_BUTTON_RIGHT;
    case MouseButton::SideFront: return SDL_BUTTON_X1;
    case MouseButton::SideBack: return SDL_BUTTON_X2;
    default: return 0;
    }
}

void Mouse::SetRelativeMotion(glm::vec2 motion)
{
    _relative = motion;
}

void Mouse::SetLocation(glm::vec2 location)
{
    _location = location;
}

void Mouse::SetMouseButtonFlag(int pos, bool value)
{
    const std::size_t bits = sizeof(int) * 8;
    if (pos >= bits)
        return;

    unsigned int mask = static_cast<unsigned int>(1) << pos;

    if (value) {
        // turn the bit on
        _buttonFlags |= mask;
    } else {
        // turn the bit off
        _buttonFlags &= ~mask;   // complement flips the 1 to 0, leaving others unchanged
    }
}

void Mouse::SetWheel(glm::vec2 wheel)
{
    _wheel = wheel;
}


void Mouse::SetCaptured(Window* window, bool relative)
{
    SDL_SetWindowRelativeMouseMode(window->Get(), relative);
}

bool Mouse::GetCaptured(Window* window)
{
    return SDL_GetWindowRelativeMouseMode(window->Get());
}

bool Mouse::IsButtonDown(MouseButton button)
{
    return _buttonFlags & SDL_BUTTON_MASK(MouseButtonToSDL(button));
}

glm::vec2 Mouse::GetMousePosition()
{
    return _location;
}

glm::vec2 Mouse::GetMouseDelta()
{
    return _relative;
}

glm::vec2 Mouse::GetMouseScrollDelta()
{
    return _wheel;
}

}