//===========================//
#include <stdexcept>
//===========================//
#include "WindowPlatformGLFW.hpp"
//===========================//

CWindowPlatformGLFW::CWindowPlatformGLFW()
{
    if (glfwInit() != GLFW_TRUE)
    {
        throw std::runtime_error("Could not initialize window platform GLFW!");
    }
}

CWindowPlatformGLFW::~CWindowPlatformGLFW()
{
    glfwTerminate();
}