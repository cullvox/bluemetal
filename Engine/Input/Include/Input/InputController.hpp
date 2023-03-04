#pragma once

#include "Input/InputEvents.hpp"
#include "Input/InputSystem.hpp"

#include <string>

namespace bl {

struct InputActionBindInfo
{
    InputAction actionType;
    union action
    {
        KeyEvent key;
        MouseButtonEvent mouse;
        GamepadButtonEvent gamepad;
    };
};

struct InputAxisBindInfo
{
    InputAxis axisType;
};

template<class TClass>
class InputActionDelegate
{
public:
    typedef void (TClass::*DelegateFunction)();

    void Execute();
    
private:
    DelegateFunction function;
    TClass* pObject;
};

// Handles individual inputs for actions/axis as bindable for player controls
class InputController
{
public:
    InputController(InputSystem& system);
    ~InputController();

    void registerAction(const std::string& name, InputActionBindInfo& binding);
    void registerAxis(const std::string& name, InputAxisBindInfo& binding);

    template<class T>
    void bindAction(const std::string& name, T* pObject, void(T::pCallback)());

private:
    struct InputAction
    {
        int joystickId;

    };

    struct InputAxis
    {
        int joystickId;
        Vector3f axis;
    };

};
} // namespace bl