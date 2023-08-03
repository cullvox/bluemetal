#pragma once

#include "Export.h"
#include "Graphics/Instance.h"
#include "Graphics/Device.h"
#include "Graphics/Swapchain.h"
#include "Graphics/FrameCounter.h"
#include "Graphics/Window/Window.h"
#include "Graphics/Render/Renderer.h"

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
    GraphicsSubsystem(Engine& engine);

    /// Default Destructor
    ~GraphicsSubsystem();

    /// Initializes the graphics subsystem with this info.
    void init(const GraphicsSubsystemInitInfo* pInitInfo);
    
    /// Shuts down the render subsystem.
    void shutdown();

    /// Gets the physical devices that this system supports.
    /// Can be called before selectPhysicalDevices().
    ///
    std::vector<std::shared_ptr<PhysicalDevice>> getPhysicalDevices();

    /// Before the graphics subsystem can be used, a physical device must be selected.
    /// This step can only be skipped if @ref GraphicsSubsystemInitInfo::physicalDeviceIndex is not nullopt.
    void selectPhysicalDevice(std::shared_ptr<PhysicalDevice> physicalDevice);

    /// Gets the Vulkan instance object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    std::shared_ptr<Instance> getInstance();
    
    /// Gets the Vulkan physical device object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    std::shared_ptr<PhysicalDevice> getPhysicalDevice();

    /// Gets the Window object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    std::shared_ptr<Window> getWindow();
    
    /// Gets the Vulkan logical device object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    std::shared_ptr<Device> getDevice();

private:
    Engine&                             m_engine;
    std::shared_ptr<Instance>           m_instance;
    std::shared_ptr<PhysicalDevice>     m_physicalDevice;
    std::shared_ptr<Window>             m_window;
    std::shared_ptr<Device>             m_device;
    std::shared_ptr<Swapchain>          m_swapchain;
    std::shared_ptr<Renderer>           m_renderer;
    FrameCounter                        m_frameCounter;
    std::queue<std::function<void()>>   m_postRenderCommands;
};

} // namespace bl