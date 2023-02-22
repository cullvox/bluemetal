#pragma once

#include "Window/VideoMode.hpp"
#include "core/Version.hpp"


#include <glm/vec2.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <string>


namespace bl
{

class Window
{
public:
    Window();
    Window(VideoMode videoMode, std::string title = "Window");
    ~Window();

    void create(VideoMode videoMode = VideoMode::getDesktopMode(), std::string title = "Window");
    
    glm::ivec2 getExtent() noexcept;
    bool createVulkanSurface(VkInstance& instance, VkSurfaceKHR& surface) noexcept;

private:
    static GLFWwindow* m_pWindow;

};

inline Window g_window{VideoMode::getDesktopMode(), applicationName};

}; /* namespace bl */