#pragma once

#include "Export.h"

#include "Engine/Subsystem.h"
#include "Graphics/RenderDoc.h"
#include "Graphics/Instance.h"
#include "Graphics/RenderWindow.h"
#include "Graphics/Device.h"
#include "Graphics/DescriptorSetLayoutCache.h"
#include "Graphics/PipelineLayoutCache.h"
#include "Graphics/Swapchain.h"
#include "Graphics/FrameCounter.h"
#include "Graphics/Renderer.h"

namespace bl
{

class Engine;

class GraphicsSubsystem
{
public:
    /// Engine Constructor
    GraphicsSubsystem(Engine& engine);

    /// Default Destructor
    ~GraphicsSubsystem();

    std::shared_ptr<RenderDoc> getRenderDoc();

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
    std::shared_ptr<RenderWindow> getWindow();
    
    /// Gets the Vulkan logical device object.
    ///
    /// This function must not be called before @ref selectPhysicalDevice is called. 
    std::shared_ptr<GfxDevice> getDevice();

    std::shared_ptr<DescriptorSetLayoutCache> getDescriptorCache();
    std::shared_ptr<PipelineLayoutCache> getPipelineLayoutCache();

    std::shared_ptr<Swapchain> getSwapchain();

    /// Gets the renderer object to record draw commands onto.
    std::shared_ptr<Renderer> getRenderer();

private:
    Engine&                                     _engine;
    std::shared_ptr<RenderDoc>                  _renderDoc;
    std::shared_ptr<GfxInstance>                _instance;
    std::shared_ptr<GfxPhysicalDevice>          _physicalDevice;
    std::shared_ptr<RenderWindow>               _window;
    std::shared_ptr<GfxDevice>                  _device;
    std::shared_ptr<DescriptorSetLayoutCache>   _descriptorCache;
    std::shared_ptr<PipelineLayoutCache>        _pipelineLayoutCache;
    std::shared_ptr<Swapchain>               _swapchain;
    std::shared_ptr<Renderer>                   _renderer;
    FrameCounter                                _frameCounter;
    std::queue<std::function<void()>>           _postRenderCommands;
};

} // namespace bl