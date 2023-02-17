#include "WindowGLFW.hpp"
#include "GLFW/glfw3.h"
#include "core/Version.hpp"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

CWindowGLFW::CWindowGLFW(const std::string& title, CConfig* pConfig)
{

    int width = pConfig->GetIntValue("window.width");
    int height = pConfig->GetIntValue("window.height");

    m_pWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);


}

CWindowGLFW::~CWindowGLFW()
{
    glfwDestroyWindow(m_pWindow);
}

VkSurfaceKHR CWindowGLFW::CreateSurface(VkInstance instance)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    if (glfwCreateWindowSurface(instance, m_pWindow, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan surface!");
    }

    return surface;
}

VkExtent2D CWindowGLFW::GetExtent() const noexcept
{
    int width = 0, height = 0;
    glfwGetWindowSize(m_pWindow, &width, &height);
    return VkExtent2D{ (uint32_t)width, (uint32_t)height };
}