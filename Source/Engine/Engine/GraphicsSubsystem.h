#pragma once

#include "Export.h"
#include "Graphics/Instance.h"
#include "Graphics/Device.h"
#include "Graphics/Swapchain.h"
#include "Graphics/FrameCounter.h"
#include "Graphics/Window/Window.h"
#include "Graphics/Renderer.h"

namespace bl
{

///////////////////////////////
// Forward Declarations
///////////////////////////////

class Engine;

///////////////////////////////
// Classes
///////////////////////////////

struct GraphicsSubsystemInitInfo
{
    std::optional<uint32_t> physicalDeviceIndex;
};

class BLUEMETAL_API GraphicsSubsystem
{
public:

    /// Engine Constructor
    GraphicsSubsystem(Engine* pEngine);

    /// Default Destructor
    ~GraphicsSubsystem();

    /// Initializes the graphics subsystem with this info.
    void init(const GraphicsSubsystemInitInfo* pInitInfo);
    
    /// Shuts down the render subsystem.
    void shutdown();

    /// Gets the physical devices that this system supports.
    /// Can be called before selectPhysicalDevices().
    ///
    std::vector<std::shared_ptr<GraphicsPhysicalDevice>> getPhysicalDevices();

    /// Before the graphics subsystem can be used, a physical device must be selected.
    /// This step can only be skipped if @ref GraphicsSubsystemInitInfo::physicalDeviceIndex is not nullopt.
    void selectPhysicalDevice(GraphicsPhysicalDevice* physicalDevice);

    /// Gets the Vulkan instance object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    GraphicsInstance* getInstance();
    
    /// Gets the Vulkan physical device object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    GraphicsPhysicalDevice* getPhysicalDevice();

    /// Gets the Window object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    Window* getWindow();
    
    /// Gets the Vulkan logical device object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    GraphicsDevice* getDevice();

    /// Gets the renderer object to record draw commands onto.
    Renderer* getRenderer();

private:
    Engine*                                 m_pEngine;
    std::unique_ptr<GraphicsInstance>       m_instance;
    GraphicsPhysicalDevice*                 m_pPhysicalDevice;
    std::unique_ptr<Window>                 m_window;
    std::unique_ptr<GraphicsDevice>         m_device;
    std::unique_ptr<Swapchain>              m_swapchain;
    std::unique_ptr<Renderer>               m_renderer;
    FrameCounter                            m_frameCounter;
    std::queue<std::function<void()>>       m_postRenderCommands;
};

} // namespace bl