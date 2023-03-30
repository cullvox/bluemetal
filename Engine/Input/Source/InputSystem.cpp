#include "Core/Log.hpp"
#include "Input/InputSystem.hpp"

#include <SDL2/SDL.h>

namespace bl
{

InputSystem::InputSystem() noexcept
{
    _windowCbHandle = addHookCallback<InputSystem>(this, &InputSystem::windowEventHook);
}

InputSystem::~InputSystem() noexcept
{
    if (_hookCallbacks.size() > 0)
        removeHookCallback(_windowCbHandle);
}

InputSystem& InputSystem::operator=(InputSystem&& rhs) noexcept
{
    collapse();

    // Specialty: we must remove the callbacks because they point to this, and that is getting deleted in the other object so we need to recreate them here.
    rhs._hookCallbacks.remove(rhs._windowCbHandle);

    _windowCbHandle = std::move(rhs._windowCbHandle);
    _hookCallbacks = std::move(rhs._hookCallbacks);
    _actions = std::move(rhs._actions);
    _axes = std::move(rhs._axes);

    rhs.collapse();

    return *this;
}

bool InputSystem::registerAction(const std::string& name, const std::vector<Key>& onKeys) noexcept
{
    _actions.insert({ name, _ActionBinding{ onKeys, {} } });
    return true;
}

bool InputSystem::registerAxis(const std::string& name, const std::vector<KeyWithScale>& onAxes) noexcept
{
    _axes.insert({ name, _AxisBinding{ onAxes, {} } });
    return true;
}

auto InputSystem::addHookCallback(const InputSystem::HookCallback& callback) noexcept -> InputSystem::HookHandle
{
    return _hookCallbacks.append(callback);
}

auto InputSystem::removeHookCallback(InputSystem::HookHandle handle) noexcept -> void
{
    _hookCallbacks.remove(handle);
}

auto InputSystem::addActionCallback(const std::string& name, const InputSystem::ActionCallback& callback) noexcept -> std::optional<InputSystem::ActionHandle>
{
    const auto it = _actions.find(name);

    return it == _actions.end() ? std::nullopt : std::make_optional(it->second.callbacks.append(callback));
}

auto InputSystem::addAxisCallback(const std::string& name, const InputSystem::AxisCallback& callback) noexcept -> std::optional<InputSystem::AxisHandle>
{
    const auto it = _axes.find(name);

    return it == _axes.end() ? std::nullopt : std::make_optional(it->second.callbacks.append(callback));
}

void InputSystem::processKeyboardPump() const noexcept
{
    int keystatesCount = 0;
    const uint8_t* pKeystates = SDL_GetKeyboardState(&keystatesCount);

    // Run delegates for actions and axes if they have keyboard keys
    for (const auto& action : _actions)
        for (auto key : action.second.triggerKeys)
            if (IsKeyFromKeyboard(key) && pKeystates[SDL_GetScancodeFromKey((SDL_Keycode)key)] == SDL_PRESSED)
                action.second.callbacks();

    for (const auto& axis : _axes)
        for (auto key : axis.second.triggerAxes)
            if (IsKeyFromKeyboard(key.first) && pKeystates[SDL_GetScancodeFromKey((SDL_Keycode)key.first)] == SDL_PRESSED)
                axis.second.callbacks(key.second);
}

void InputSystem::windowEventHook(const SDL_Event* pEvent)
{
    if (not pEvent) return;
    if (pEvent->type != SDL_WINDOWEVENT) return;

    const auto pWindowEvent = &pEvent->window;
    const auto windowKey = GetWindowKeyFromSDL(pWindowEvent->event);

    if (windowKey == Key::KeyUnknown) return;

    // Run the action delegates, window keys do not support axis events.
    for (const auto& action : _actions)
        for (auto key : action.second.triggerKeys)
            if (key == windowKey)
                action.second.callbacks();

}

void InputSystem::poll() noexcept
{
    
    SDL_PumpEvents();

    processKeyboardPump();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        _hookCallbacks(&event);
    }
}

void InputSystem::collapse() noexcept
{
    _hookCallbacks.clear();
    _actions.clear();
    _axes.clear();
}

} // namespace bl