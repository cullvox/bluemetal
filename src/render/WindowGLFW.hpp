#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Window.hpp"
#include "config/Configurable.hpp"

class CWindowGLFW : public IWindow
{
public:
    CWindowGLFW(const std::string& title, VkExtent2D extent, CConfig* pConfig = g_pConfig.get());
    ~CWindowGLFW();

    VkSurfaceKHR CreateSurface(VkInstance instance);
    VkExtent2D GetExtent() const noexcept;
private:
    GLFWwindow* m_pWindow;
};