#pragma once

#include "Math/Math.h"

namespace bl {

enum class GamepadButton : uint32_t {
    A,
    B,
    X,
    Y,
    PadUp,
    PadDown,
    PadLeft,
    PadRight,
    LeftShoulder,
    RightShoulder,
    LeftStick,
    RightStick,
};

enum class GamepadAnalog : uint32_t {
    LeftJoystick,
    RightJoystick,
    LeftTrigger,
    RightTrigger
};

class Gamepad {
public:
    /// @brief Destructor
    virtual ~Gamepad() = default;

    /// @brief Returns the state of a button 
    virtual bool GetButtonDown(GamepadButton button) = 0;

    /// @brief Returns an analog input from a gamepad.
    virtual glm::vec2 GetAnalog(GamepadAnalog analog) = 0;
};


} // namespace bl