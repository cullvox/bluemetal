#include "Input/InputSystem.hpp"

#include <SDL2/SDL.h>

namespace bl
{

InputSystem::InputSystem(Window& window)
    : m_window(window)
{
    
}

InputSystem::~InputSystem()
{
    glfwSetWindowUserPointer(m_window.getHandle(), nullptr);
    glfwSetCursorPosCallback(m_window.getHandle(), nullptr);
    glfwSetKeyCallback(m_window.getHandle(), nullptr);
}

void InputSystem::registerAction(const std::string& name, std::vector<Key> keys)
{
    m_actionsEvents.emplace_back(InputAction{name, keys});
}

void InputSystem::registerAxis(const std::string& name, std::vector<std::pair<Key, float>> axes)
{
    m_axisEvents.emplace_back(InputAxis{name, axes});
}

void InputSystem::registerController(InputController& controller)
{
    m_inputControllers.push_back(&controller);
}

void InputSystem::poll()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_WINDOWEVENT:
                
        }
    }
}

void InputSystem::keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{

    // Get the bl::InputSystem from the user pointer.
    InputSystem* pInput = static_cast<InputSystem*>(glfwGetWindowUserPointer(pWindow));
    if (pInput == nullptr) return;

    // Keyboard keys are directly convertible to bl::Key.
    InputEvent event = InputEvent::Pressed;
    switch (action)
    {
        case GLFW_REPEAT:
        case GLFW_PRESS:
            event = InputEvent::Pressed;
            break;
        case GLFW_RELEASE:
            event = InputEvent::Released;
            break;
        default:
            event = InputEvent::Released;
    }

    // Set the keys InputEvent value
    pInput->m_keyValues[key - (int)Key::KeyboardFirst] = event;

    // TODO: This is a little bit of a hacky way to check but it works
    for (int i = 0; i < (int)Key::KeyboardSize; i++)
    {
        // Run action events
        pInput->onActionEvent((Key)(i+(int)Key::KeyboardFirst), pInput->m_keyValues[i]);

        // On axis events only run when pressed
        if (pInput->m_keyValues[i] == InputEvent::Pressed)
            pInput->onAxisEvent((Key)(i+(int)Key::KeyboardFirst), 1.0f);
    }
}

void InputSystem::mouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods)
{
    // Get the bl::InputSystem from the user pointer.
    InputSystem* pInput = static_cast<InputSystem*>(glfwGetWindowUserPointer(pWindow));
    if (pInput == nullptr) return;


}

void InputSystem::cursorPositionCallback(GLFWwindow* pWindow, double xpos, double ypos)
{

    // Get the bl::Window from the user pointer.
    InputSystem* pInput = static_cast<InputSystem*>(glfwGetWindowUserPointer(pWindow));
    if (pInput == nullptr) return;

    pInput->onAxisEvent(Key::MouseXAxis, (float)xpos);
    pInput->onAxisEvent(Key::MouseYAxis, (float)ypos);
}

void InputSystem::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
}

void InputSystem::windowCloseCallback(GLFWwindow* pWindow)
{
    // Get the bl::InputSystem from the user pointer.
    InputSystem* pInput = static_cast<InputSystem*>(glfwGetWindowUserPointer(pWindow));
    if (pInput == nullptr) return;

    pInput->onActionEvent(bl::Key::WindowClose, bl::InputEvent::Pressed);
}

void InputSystem::onActionEvent(Key key, InputEvent event)
{
    for (InputAction& action : m_actionsEvents)
    {
        if (std::find(action.keys.begin(), action.keys.end(), key) == action.keys.end()) continue;
        for (InputController* pInputController : m_inputControllers)
        {
            pInputController->onActionEvent(action.name, event);
        }
    }
}

void InputSystem::onAxisEvent(Key key, float axisValue)
{
    for (InputAxis& axis : m_axisEvents)
    {
        auto keyPair = std::find_if(axis.keys.begin(), axis.keys.end(), [key](auto& pair){ return pair.first == key; });
        if (keyPair == axis.keys.end()) continue;
        for (InputController* pInputController : m_inputControllers)
        {
            pInputController->onAxisEvent(axis.name, axisValue * (*keyPair).second);
        }
    }
}

} // namespace bl