#pragma once

//===========================//
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//===========================//
#include "render/WindowPlatform.hpp"
//===========================//

class CWindowPlatformGLFW : public virtual IWindowPlatform
{
public:
    CWindowPlatformGLFW();
    ~CWindowPlatformGLFW();
    std::shared_ptr<IWindow> CreateWindow(const std::string& title, CConfig* const pConfig = g_pConfig.get()); 
    const std::vector<std::shared_ptr<IMonitor>>& GetMonitors();
};