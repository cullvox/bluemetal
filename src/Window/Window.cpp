#include "Window/Window.hpp"

#include <stdexcept>


namespace bl {

Window::Window(VideoMode videoMode, std::string title)
{
    create(videoMode, title);
}

Window::~Window()
{
    if (m_pWindow) glfwDestroyWindow(m_pWindow);
}

void Window::create(VideoMode videoMode, std::string title)
{
    std::string windowName = (title == applicationName) ? "window" : "window-" + title; 

    /* Apply the window hints for vulkan. */
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_pWindow = glfwCreateWindow(videoMode.resolution.width, videoMode.resolution.height, title.c_str(), nullptr, nullptr);
    if (!m_pWindow)
    {
        throw std::runtime_error("Could not create the GLFW window!");       
    }

    glfwMakeContextCurrent(m_pWindow);
}

bool Window::createVulkanSurface(VkInstance instance, VkSurfaceKHR& surface) noexcept
{
    return glfwCreateWindowSurface(instance, m_pWindow, nullptr, &surface) == VK_SUCCESS;
}

Extent2D Window::getExtent() const noexcept
{
    int width = 0, height = 0;
    glfwGetWindowSize(m_pWindow, &width, &height);
    return Extent2D{(uint32_t)width, (uint32_t)height};
}

const std::vector<const char*>& Window::getSurfaceExtensions()
{
    uint32_t extensionsCount = 0;
    const char** ppExtensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

    static std::vector<const char*> instanceExtensions{ppExtensions, ppExtensions + extensionsCount};
    
    return instanceExtensions;
}

} /* namespace bl; */