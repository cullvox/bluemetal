#include "Input/InputSystem.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace bl
{

InputSystem::InputSystem(Window& window)
    : m_window(window)
{
    glfwSetKeyCallback(m_window.getHandle(), keyCallback);
    glfwSetCursorPosCallback(m_window.getHandle(), cursorPositionCallback);
    glfwSetScrollCallback(m_window.getHandle(), scrollCallback);
    glfwSetMouseButtonCallback(m_window.getHandle(), mouseButtonCallback);
    glfwSetWindowUserPointer(m_window.getHandle(), this);
}

InputSystem::~InputSystem()
{
    glfwSetWindowUserPointer(m_window.getHandle(), nullptr);
    glfwSetCursorPosCallback(m_window.getHandle(), nullptr);
    glfwSetKeyCallback(m_window.getHandle(), nullptr);
}

void InputSystem::poll()
{
    glfwPollEvents();
}

void InputSystem::keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{

    // Get the bl::InputSystem from the user pointer.
    InputSystem* pInput = static_cast<InputSystem*>(glfwGetWindowUserPointer(pWindow));
    if (pInput == nullptr) return;

    // Run the callback
    

}

void InputSystem::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{

    // Get the bl::Window from the user pointer.
}

} // namespace bl