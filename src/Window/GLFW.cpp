#include "GLFW.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdexcept>

GLFW::GLFW()
{
    if (glfwInit() != GLFW_TRUE)
    {
        throw std::runtime_error("Could not initialize GLFW!");
    }
}

GLFW::~GLFW()
{
    glfwTerminate();
}

GLFW glfw{};