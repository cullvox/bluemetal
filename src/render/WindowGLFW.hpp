#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Window.hpp"
#include "config/Configurable.hpp"

class CWindowGLFW : public virtual IWindow
{
public:
    CWindowGLFW(const std::string& title, CConfig* pConfig = g_pConfig.get());
    ~CWindowGLFW();

    virtual VkSurfaceKHR CreateSurface(VkInstance instance);
    virtual VkExtent2D GetExtent() const noexcept;
private:
    GLFWwindow* m_pWindow;
};