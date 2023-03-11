#pragma once

#include "Core/Export.h"
#include "Core/Delegate.hpp"
#include "Input/InputEvents.hpp"

#include <memory>
#include <vector>
#include <string>

namespace bl {

using ActionDelegate = Delegate<void(void)>;
using AxisDelegate = Delegate<void(float)>;

struct InputActionBinding
{
    std::string name;
    InputEvent event;
    ActionDelegate actionDelegate;
};

struct InputAxisBinding
{
    InputAxisBinding() = default;
    InputAxisBinding(std::string name, AxisDelegate axisDelegate, float scale)
        : name(name), axisDelegate(axisDelegate), scale(scale){ }

    std::string name;
    AxisDelegate axisDelegate;
    float scale;
};

// Handles individual inputs for actions/axis as bindable for player controls
class BLOODLUST_API InputController
{
public:

    InputController();
    ~InputController();

    void bindAction(const std::string& name, InputEvent event, ActionDelegate::FunctionType function);

    template<class TClass>
    void bindAction(const std::string& name, InputEvent event, TClass& instance, void(TClass::* function)(void))
    {
        m_actionBindings.emplace_back(InputActionBinding{name, event, ActionDelegate(instance, function)});
    }

    template<class TClass>
    void bindAxis(const std::string& name, TClass& instance, void(TClass::* function)(float))
    {
        auto delegate = AxisDelegate(instance, function);
        m_axisBindings.emplace_back(InputAxisBinding(name, delegate, 0.0f));
    }

    void onActionEvent(const std::string& name, InputEvent input);
    void onAxisEvent(const std::string& name, float axis);

private:
    std::vector<InputActionBinding> m_actionBindings;
    std::vector<InputAxisBinding> m_axisBindings;

    friend class InputSystem;
};
} // namespace bl