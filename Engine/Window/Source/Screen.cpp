#include "Math/Vector2.hpp"
#include "Window/VideoMode.hpp"
#include "Window/Screen.hpp"

namespace bl {

Screen::Screen(GLFWmonitor* pMonitor, unsigned int screen, const std::string& name, Extent2D resolution, Vector2f contentScale, unsigned int refreshRate, const std::vector<VideoMode>& videoModes, VideoMode desktopMode)
    : m_pMonitor(pMonitor), screen(screen), name(name), resolution(resolution), contentScale(contentScale), refreshRate(refreshRate), m_videoModes(videoModes), m_desktopMode(desktopMode)
{
}

Screen::~Screen()
{
}

const std::vector<VideoMode>& Screen::getVideoModes() const
{
    return m_videoModes;
}

VideoMode Screen::getDesktopMode() const
{
    return m_desktopMode;
}


GLFWmonitor* Screen::getHandle() const noexcept
{
    return m_pMonitor;
}

Screen Screen::getScreen(unsigned int index)
{
    return getScreens()[index];
}

std::vector<Screen> Screen::getScreens()
{

    /* Get the GLFW monitors. */
    int monitorCount = 0;
    GLFWmonitor** ppMonitors = glfwGetMonitors(&monitorCount);

    std::vector<Screen> screens;

    for (int i = 0; i < monitorCount; i++)
    {
        GLFWmonitor* pMonitor = ppMonitors[i];

        /* Get the monitor name. */
        std::string name = glfwGetMonitorName(pMonitor);
    
        /* Get the video modes. */
        std::vector<VideoMode> videoModes{};

        int videoModeCount = 0;
        const GLFWvidmode* pVideoModes = glfwGetVideoModes(pMonitor, &videoModeCount);        

        for (int j = 0; j < videoModeCount; j++)
        {
            const GLFWvidmode* pVideoMode = &pVideoModes[j];
            const VideoMode videoMode{
                Extent2D{
                    (unsigned int)pVideoMode->width, 
                    (unsigned int)pVideoMode->height},
                (unsigned int)pVideoMode->redBits,
                (unsigned int)pVideoMode->refreshRate,
            };
            
            videoModes.emplace_back(videoMode);
        }

        /* The last video mode is the highest res. */
        VideoMode bestMode = videoModes[0];

        Vector2f contentScale;
        glfwGetMonitorContentScale(pMonitor, &contentScale.x, &contentScale.y);

        Extent2D desktopModeResolution;
        glfwGetMonitorWorkarea(pMonitor, nullptr, nullptr, (int*)&desktopModeResolution.width, (int*)&desktopModeResolution.height);

        VideoMode desktopMode{desktopModeResolution, bestMode.bitsPerPixel, bestMode.refreshRate};

        const Screen screen{pMonitor, (unsigned int)i, name, bestMode.resolution, contentScale, bestMode.refreshRate, videoModes, desktopMode};

        screens.emplace_back(screen);
    }

    return screens;
}

};