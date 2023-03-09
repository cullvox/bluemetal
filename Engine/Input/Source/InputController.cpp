#include "Input/InputController.hpp"

namespace bl
{

InputController::InputController()
{
}

InputController::~InputController()
{
}

void InputController::bindAction(const std::string& name, InputEvent event, ActionDelegate::FunctionType function)
{
    ActionDelegate delegate{};
    delegate.bind(function);
    m_actionBindings.emplace_back(InputActionBinding{ name, event, delegate });
}

void InputController::onActionEvent(const std::string& name, InputEvent input)
{
    // Look if we have any bindings matching the name and input description.
    for (InputActionBinding& binding : m_actionBindings)
    {
        if (binding.name != name) continue;
        if (binding.event != input) continue;
        
        // Run the delegate.
        else binding.actionDelegate.invoke(); 
    }
}

void InputController::onAxisEvent(const std::string& name, float axis)
{
    for (InputAxisBinding& binding : m_axisBindings)
    {
        if (binding.name != name) continue;
        else binding.axisDelegate.invoke(axis);
    }
}

} // namespace bl