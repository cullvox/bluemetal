#include "Core/Log.hpp"
#include "Input/InputSystem.hpp"

#include <SDL2/SDL.h>

namespace bl
{

InputSystem::InputSystem() noexcept
{
    m_hookDelegate += HookDelegate(*this, &InputSystem::windowEventHook);   
}

InputSystem::~InputSystem() noexcept
{
    m_hookDelegate.remove(m_hookDelegate, HookDelegate(*this, &InputSystem::windowEventHook));
}

InputSystem& InputSystem::operator=(InputSystem&& rhs) noexcept
{
    collapse();

    m_hookDelegate = rhs.m_hookDelegate;
    m_actions = rhs.m_actions;
    m_axes = rhs.m_axes;

    rhs.collapse();

    return *this;
}

bool InputSystem::registerAction(const std::string& name, const std::vector<Key>& onKeys) noexcept
{
    m_actions.insert({ std::string{name}, ActionBinding{ onKeys, {} } });
    return true;
}

bool InputSystem::registerAxis(const std::string& name, const std::vector<Axis>& onAxes) noexcept
{
    m_axes.insert({ std::string{name}, AxisBinding{ onAxes, {} } });
    return true;
}

InputSystem::HookDelegate& InputSystem::getHookDelegate(void) noexcept
{
    return m_hookDelegate;
}

InputSystem::ActionDelegate* InputSystem::getActionDelegate(const std::string& name) noexcept
{
    const auto it = m_actions.find(name);

    return it == m_actions.end() ? nullptr : &it->second.delegate;
}

InputSystem::AxisDelegate* InputSystem::getAxisDelegate(const std::string& name) noexcept
{
    const auto it = m_axes.find(name);

    return it == m_axes.end() ? nullptr : &it->second.delegate;
}

void InputSystem::processKeyboardPump() const noexcept
{
    int keystatesCount = 0;
    const uint8_t* pKeystates = SDL_GetKeyboardState(&keystatesCount);

    // Run delegates for actions and axes if they have keyboard keys
    for (const auto& action : m_actions)
        for (auto key : action.second.onKeys)
            if (IsKeyFromKeyboard(key) && pKeystates[SDL_GetScancodeFromKey((SDL_Keycode)key)] == SDL_PRESSED)
                action.second.delegate();

    for (const auto& axis : m_axes)
        for (auto key : axis.second.onAxes)
            if (IsKeyFromKeyboard(key.first) && pKeystates[SDL_GetScancodeFromKey((SDL_Keycode)key.first)] == SDL_PRESSED)
                axis.second.delegate(key.second);
}

void InputSystem::windowEventHook(const SDL_Event* pEvent) const noexcept
{
    if (pEvent->type != SDL_WINDOWEVENT) return;

    const auto pWindowEvent = &pEvent->window;
    const auto windowKey = GetWindowKeyFromSDL(pWindowEvent->event);

    if (windowKey == Key::KeyUnknown) return;

    // Run the action delegates, window keys do not support axis events.
    for (const auto& action : m_actions)
        for (auto key : action.second.onKeys)
            if (key == windowKey)
                action.second.delegate();

}

void InputSystem::poll() noexcept
{
    
    SDL_PumpEvents();

    processKeyboardPump();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        m_hookDelegate(&event);
    }
}

void InputSystem::collapse() noexcept
{
    m_hookDelegate.clear();
    m_actions.clear();
    m_axes.clear();
}

} // namespace bl