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
    bool init(const GraphicsSubsystemInitInfo* pInitInfo);
    
    /// Shuts down the render subsystem.
    void shutdown();

    /// Gets the physical devices that this system supports.
    /// Can be called before selectPhysicalDevices().
    ///
    std::vector<std::shared_ptr<GfxPhysicalDevice>> getPhysicalDevices();

    /// Before the graphics subsystem can be used, a physical device must be selected.
    /// This step can only be skipped if @ref GraphicsSubsystemInitInfo::physicalDeviceIndex is not nullopt.
    void selectPhysicalDevice(GfxPhysicalDevice* physicalDevice);

    /// Gets the Vulkan instance object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    std::shared_ptr<GfxInstance> getInstance();
    
    /// Gets the Vulkan physical device object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    std::shared_ptr<GfxPhysicalDevice> getPhysicalDevice();

    /// Gets the Window object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    std::shared_ptr<Window> getWindow();
    
    /// Gets the Vulkan logical device object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    std::shared_ptr<GfxDevice> getDevice();

    /// Gets the renderer object to record draw commands onto.
    std::shared_ptr<Renderer> getRenderer();

private:
    Engine*                                 _engine;
    std::shared_ptr<GfxInstance>            _instance;
    std::shared_ptr<GfxPhysicalDevice>      _physicalDevice;
    std::shared_ptr<Window>                 _window;
    std::shared_ptr<GfxDevice>              _device;
    std::shared_ptr<GfxSwapchain>           _swapchain;
    std::shared_ptr<Renderer>               _renderer;
    FrameCounter                            _frameCounter;
    std::queue<std::function<void()>>       _postRenderCommands;
};

} // namespace bl
