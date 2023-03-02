#include "Input.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace bl
{

void Input::poll()
{
    glfwPollEvents();
}

} // namespace bl