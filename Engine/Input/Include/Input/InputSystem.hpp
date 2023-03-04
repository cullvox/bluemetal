#pragma once

#include "Window/Window.hpp"
#include "Input/InputEvents.hpp"
#include "Input/InputController.hpp"

namespace bl
{


// Handles input callbacks from windows
class InputSystem
{
public:
    InputSystem(Window& window);
    ~InputSystem();
    void addController(InputController& controller);
    void removeController(InputController& controller);
    
    static void poll();
private:

    // GLFW callbacks
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    Window& m_window;
    std::vector<InputController*> inputControllers;
};

} // namespace bl