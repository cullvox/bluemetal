#include "WindowPlatformGLFW.h"

namespace bl {

WindowPlatformGLFW::WindowPlatformGLFW() {
    if (glfwInit() != GLFW_TRUE) {
        throw std::runtiem 
    }
}

WindowPlatformGLFW::~WindowPlatformGLFW() {

}

}