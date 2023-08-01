#pragma once


#include "Export.h"
#include "Noodle/Noodle.h"
#include "Window/Window.h"
#include "Graphics/Instance.h"
#include "Graphics/Device.h"
#include "Graphics/Swapchain.h"
#include "Graphics/FrameCounter.h"
#include "Render/Renderer.h"

namespace bl
{

class BLUEMETAL_API Engine
{
public:
    /// Returns the singleton instance of the engine. 
    static Engine& get();

public:

    /// Constructs the engine building the engine up to a working state.
    Engine();

    /// Destroys the entire engine and shuts everything down.
    ~Engine();

    /// Returns the config noodle.
    Noodle& getConfig() { return m_config; }

    /// Returns the 
    std::shared_ptr<Instance> getInstance() { return m_instance; }
    std::shared_ptr<Device> getDevice() { return m_device; }
    std::shared_ptr<PhysicalDevice> getPhysicalDevice() { return m_physicalDevice; }
    std::shared_ptr<Window> getWindow() { return m_window; }
    std::shared_ptr<Device> getDevice() { return m_device; }
    std::shared_ptr<Swapchain> getSwapchain() { return m_swapchain; }
    std::shared_ptr<Renderer> getRenderer() { return m_renderer; }

    /// Begins the engines game loop.
    void run();
    
private:
    bool                                m_shouldClose;
    bool                                m_showImGui;
    bool                                m_fullscreen;
    Noodle                              m_config;
    std::shared_ptr<Instance>           m_instance;
    std::shared_ptr<PhysicalDevice>     m_physicalDevice;
    std::shared_ptr<Window>             m_window;
    std::shared_ptr<Device>             m_device;
    std::shared_ptr<Swapchain>          m_swapchain;
    std::shared_ptr<Renderer>           m_renderer;
    FrameCounter                        _frameCounter;
    std::queue<std::function<void()>>   _postRenderCommands;
};

} // namespace bl