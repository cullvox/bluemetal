#pragma once

#include "Window/Window.hpp"
#include "Input/InputEvents.hpp"
#include "Input/InputController.hpp"

#include <array>
#include <vector>

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
    InputSystem();
    ~InputSystem();
    
    void registerAction(const std::string& name, std::vector<Key> keys);
    void registerAxis(const std::string& name, std::vector<std::pair<Key, float>> axes); // (key, scale)

    void registerController(InputController& controller);

    void poll();
private:
    void processKeyboard();
    void onActionEvent(Key key, InputEvent event);
    void onAxisEvent(Key key, float value);

    std::vector<InputAction> m_actionsEvents;
    std::vector<InputAxis> m_axisEvents;
    std::vector<InputController*> m_inputControllers;
};

} // namespace bl