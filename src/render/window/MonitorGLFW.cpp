//===========================//
#include "MonitorGLFW.hpp"
//===========================//

CMonitorGLFW::CMonitorGLFW(GLFWmonitor* pMonitor)
{
    int videoModeCount = 0;
    const GLFWvidmode* pVideoModes = glfwGetVideoModes(pMonitor, &videoModeCount);
    
    for (uint32_t i = 0; i < videoModeCount; i++)
    {
        const GLFWvidmode* pMode = &pVideoModes[i];
        const SVideoMode newMode{
            .width = pMode->width,
            .height = pMode->height,
            .bpp = pMode->redBits,
            .rr = pMode->refreshRate
        };
        m_videoModes.emplace_back(newMode);
    }
}

const std::vector<SVideoMode>& CMonitorGLFW::GetVideoModes()
{
    return m_videoModes;
}