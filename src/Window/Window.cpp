#include "Window/Window.hpp"
#include "GLFW/glfw3.h"
#include "Window/Screen.hpp"

#include <stdexcept>


namespace bl {

Window::Window()
{
}

Window::Window(VideoMode videoMode, std::optional<Screen> screen, std::string title)
{
    create(videoMode, screen, title);
}

Window::~Window()
{
    if (m_pWindow) glfwDestroyWindow(m_pWindow);
}

void Window::create(VideoMode videoMode, std::optional<Screen> screen, std::string title)
{

    // Apply the window hints for vulkan.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Create the GLFW window
    m_pWindow = glfwCreateWindow(videoMode.resolution.width, videoMode.resolution.height, title.c_str(), nullptr, nullptr);
    if (!m_pWindow)
    {
        throw std::runtime_error("Could not create the GLFW window!");       
    }

    glfwShowWindow(m_pWindow);
}

bool Window::createVulkanSurface(VkInstance instance, VkSurfaceKHR& surface) noexcept
{
    return glfwCreateWindowSurface(instance, m_pWindow, nullptr, &surface) == VK_SUCCESS;
}

Extent2D Window::getExtent() const noexcept
{
    int width = 0, height = 0;
    glfwGetWindowSize(m_pWindow, &width, &height);
    return Extent2D{(unsigned long)width, (unsigned long)height};
}

const std::vector<const char*>& Window::getSurfaceExtensions()
{
    uint32_t extensionsCount = 0;
    const char** ppExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

    /* I don't like having to make this static, but it really isn't that bad for a function guaranteed to be used only once. */
    static std::vector<const char*> instanceExtensions{ppExtensions, ppExtensions + extensionsCount};
    
    return instanceExtensions;
}

} /* namespace bl */