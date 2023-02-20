//===========================//
#include <cstdint>
#include <stdexcept>
//===========================//
#include "core/Version.hpp"
#include "WindowGLFW.hpp"
//===========================//

CWindowGLFW::CWindowGLFW(const std::string& title, CConfig* pConfig)
{

    std::string windowName = (title == applicationName) ? "window" : "window-" + title; 

    /* Get the configured options. */
    int width = pConfig->GetIntOrSetDefault(windowName + ".width", IWindow::DEFAULT_WIDTH);
    int height = pConfig->GetIntOrSetDefault(windowName + ".height", IWindow::DEFAULT_HEIGHT);
    bool fullscreen = (bool)pConfig->GetIntOrSetDefault(windowName + ".fullscreen", (int)IWindow::DEFAULT_FULLSCREEN);

    /* Apply the window hints for vulkan. */
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_pWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_pWindow)
    {
        throw std::runtime_error("Could not create the GLFW window!");       
    }

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
    return VkExtent2D{(uint32_t)width, (uint32_t)height};
}