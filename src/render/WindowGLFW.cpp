#include "WindowGLFW.hpp"

CWindowGLFW::CWindowGLFW(const std::string& title, VkExtent2D extent, CConfig* pConfig)
{
    if (pConfig->HasValue("window.fullscreen"))
    {
        
    }

    glfwCreateWindow(extent.width, extent.height, title.c_str(), nullptr, nullptr);
}