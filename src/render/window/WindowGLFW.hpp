#pragma once

//===========================//
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//===========================//
#include "config/Configurable.hpp"
#include "render/Window.hpp"
//===========================//

class CWindowGLFW
{
public:
    CWindowGLFW(const std::string& title, CConfig* pConfig = g_pConfig.get());
    ~CWindowGLFW();

    VkSurfaceKHR CreateSurface(VkInstance instance);
    VkExtent2D GetExtent() const noexcept;
private:
    GLFWwindow* m_pWindow;
};