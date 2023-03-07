#pragma once

#include "Core/Precompiled.hpp"
#include "Window/Window.hpp"
#include "Input/InputEvents.hpp"
#include "Input/InputController.hpp"

namespace bl
{

struct InputAction
{
    std::string name;
    std::vector<Key> keys;
};

struct InputAxis
{
    std::string name;
    std::vector<std::pair<Key, float>> keys;
};

// Handles input callbacks from windows
class InputSystem
{
public:
    InputSystem() = default;
    ~InputSystem() = default;
    
    void registerAction(const std::string& name, std::vector<Key> keys);
    void registerAxis(const std::string& name, std::vector<std::pair<Key, float>> axes); // (key, scale)

    void registerController(InputController& controller);

    void poll();
private:

    /// @brief Processes keyboard events and ensures repeat events are called.
    void processKeyboard();

    /// @brief Triggers input controller action events.
    /// @param key The that triggered the event.
    /// @param event What happened that triggered an event.
    void actionEvent(Key key, InputEvent event);

    
    void axisEvent(Key key, float value);
    
    /// @brief A fast way of determining key usage when iterating.
    std::bitset<SDL_NUM_SCANCODES> m_usedKeys;
    
    /// @brief Action events defined by the user.
    std::vector<InputAction> m_actionsEvents;

    /// @brief Axis events defined by the user.
    std::vector<InputAxis> m_axisEvents;

    /// @brief Registered input controllers.
    std::vector<InputController*> m_inputControllers;
};

} // namespace bl