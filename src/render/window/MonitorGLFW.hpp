#pragma once

//===========================//
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//===========================//
#include "render/Monitor.hpp"
//===========================//

class CMonitorGLFW : public IMonitor
{
public:
    CMonitorGLFW(GLFWmonitor* pMonitor);
    ~CMonitorGLFW() = default;
    virtual const std::vector<SVideoMode>& GetVideoModes() override;
private:
    std::vector<SVideoMode> m_videoModes;
};