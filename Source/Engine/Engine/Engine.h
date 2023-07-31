#pragma once


#include "Export.h"
#include "Noodle/Noodle.h"
#include "Window/Window.h"
#include "Render/Instance.h"
#include "Render/Device.h"
#include "Render/Swapchain.h"
#include "Render/Renderer.h"
#include "Render/FrameCounter.h"

namespace bl
{

class BLUEMETAL_API Engine
{
public:
    Engine(const std::string& appName);
    ~Engine();

    bool run();

private:
    bool m_shouldClose;
    bool m_showImGui;
    bool m_fullscreen;
    Noodle m_config;
    std::shared_ptr<Instance> m_instance;
    std::shared_ptr<PhysicalDevice> m_physicalDevice;
    std::shared_ptr<Window> m_window;
    std::shared_ptr<Device> m_device;
    std::shared_ptr<Swapchain> m_swapchain;
    std::shared_ptr<RenderPass> m_presentPass;
    std::shared_ptr<Renderer> m_renderer;
    FrameCounter _frameCounter;
    std::queue<std::function<void()>> _postRenderCommands;
};

} // namespace bl