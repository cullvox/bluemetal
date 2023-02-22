#include "Window/Window.hpp"

#include <stdexcept>


namespace bl {

Window::Window(VideoMode videoMode, const std::string& title)
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
    glfwShowWindow(m_pWindow);
}

Window::~Window()
{
    glfwDestroyWindow(m_pWindow);
}

VkSurfaceKHR Window::CreateSurface(VkInstance instance)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    if (glfwCreateWindowSurface(instance, m_pWindow, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan surface!");
    }

    return surface;
}

Extent2D Window::GetExtent() const noexcept
{
    int width = 0, height = 0;
    glfwGetWindowSize(m_pWindow, &width, &height);
    return VkExtent2D{(uint32_t)width, (uint32_t)height};
}

} /* namespace bl; */