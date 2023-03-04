#pragma once

namespace bl {
enum class InputDevice {
    keyboardAndMouse,
    gamePad,
};

enum class ActionType {
    key,
    mouseButton,
    gamePadButton,
};

enum class AxisType {
    mousePosition,
    gamePadStick,
}

class InputController
{
public:
    InputController(InputSystem& system);
    ~InputController();

    void registerAction();

private:
    struct InputAction
    {

    };

    struct InputAxis
    {
        Vector3f axis;
    };

};
} // namespace bl