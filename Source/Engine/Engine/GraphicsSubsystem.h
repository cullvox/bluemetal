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
    GraphicsSubsystem(Engine& engine);

    /// Default Destructor
    ~GraphicsSubsystem();

    /// Gets the physical devices that this system supports.
    /// Can be called before selectPhysicalDevices().
    ///
    std::vector<std::shared_ptr<GfxPhysicalDevice>> getPhysicalDevices();

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

    std::shared_ptr<GfxSwapchain> getSwapchain();

    /// Gets the renderer object to record draw commands onto.
    std::shared_ptr<Renderer> getRenderer();

private:
    Engine&                                 _engine;
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
