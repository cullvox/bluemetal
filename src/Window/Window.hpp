#pragma once

#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"
#include "Window/Screen.hpp"
#include "core/Version.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <string>


namespace bl
{

enum class Style
{
    eFullscreen,
    eWindowed,
    eFullscreenWindowed
};

class Window
{
public:
    Window();
    Window(VideoMode videoMode, std::optional<Screen> screen, std::string title = "Window");
    ~Window();

    void create(VideoMode videoMode, std::optional<Screen> screen, std::string title = "Window");
    Extent2D getExtent() const noexcept;
    bool createVulkanSurface(VkInstance instance, VkSurfaceKHR& surface) noexcept;

private:
    GLFWwindow* m_pWindow;

//==== Static ====//
public: 
    static const std::vector<const char*>& getSurfaceExtensions();
};

}; /* namespace bl */