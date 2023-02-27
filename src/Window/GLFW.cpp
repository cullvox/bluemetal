#include "GLFW.hpp"

#include <spdlog/spdlog.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdexcept>

GLFW::GLFW()
{
    glfwSetErrorCallback(errorCallback);
    if (glfwInit() != GLFW_TRUE)
    {
        throw std::runtime_error("Could not initialize GLFW!");
    }
}

GLFW::~GLFW()
{
    glfwTerminate();
}

void GLFW::errorCallback(int errorCode, const char* pDescription)
{
    spdlog::error("GLFW: {} | {}", errorCode, pDescription);
}

GLFW glfw{};