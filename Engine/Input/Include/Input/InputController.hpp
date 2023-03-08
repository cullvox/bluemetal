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


    void bindAction(const std::string& name, InputEvent event, void (*pFunction)(const void*), const void* data);

    template<auto TCandidate, class TClass>
    void bindAction(const std::string& name, InputEvent event, TClass* pObject)
    {
        ActionDelegate delegate{};
        delegate.connect<TCandidate, TClass>(pObject);
        m_actionBindings.emplace_back(InputActionBinding{name, event, delegate});
    }

    template<auto TCandidate, class TClass>
    void bindAxis(const std::string& name, TClass* pObject)
    {
        AxisDelegate delegate{};
        delegate.connect<TCandidate, TClass>(pObject);
        m_axisBindings.emplace_back(InputAxisBinding{name, delegate, 0.0f});
    }

    void onActionEvent(const std::string& name, InputEvent input);
    void onAxisEvent(const std::string& name, float axis);

private:
    std::vector<InputActionBinding> m_actionBindings;
    std::vector<InputAxisBinding> m_axisBindings;

    friend class InputSystem;
};
} // namespace bl