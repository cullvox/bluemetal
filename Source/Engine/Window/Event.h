#pragma once

#include "Precompiled.h"
#include "Core/Flags.h"

namespace bl {

class Keyboard;
class Mouse;
class Gamepad;
class Window;

enum class EventType {
    WindowClose, ///!< The window was asked to be closed.
    WindowResize, ///!< The window was resized.
    WindowMinimize, ///!< More specific term for window resize, use Event::WindowResizeEvent.
    WindowMaximize, ///!< More specific term for window resize, use Event::WindowResizeEvent.
    WindowFocus,
    WindowUnfocus,

    KeyboardPressed,
    KeyboardReleased,

    MouseButtonPressed,
    MouseButtonReleased,
    MouseMotion,
    MouseScroll,

    DisplayRemoved, ///!< A display was removed from the system.
    DisplayAdded, ///!< A display was added to the system. 
    DisplayChanged, ///!< A propery of the display was changed.

    GamepadAdded,
    GamepadRemoved,
    GamepadButtonPressed, ///!< A 
    GamepadButtonReleased, ///!< A button on the gamepad was released.
    GamepadAnalogMotion, ///!< A trigger or joystick state was updated.
};

/// @brief Input Interface for managing all input data sources.
class Input {
public:
    virtual ~Input() = 0;

    virtual void Poll() = 0;
    virtual Keyboard& GetKeyboard() = 0;
    virtual Mouse& GetMouse() = 0;
    virtual const std::vector<Gamepad>& GetGamepads() = 0;
};

} // namespace bl