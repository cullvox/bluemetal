#include "GraphicsSubsystem.h"
#include "Engine.h"
#include "Core/Print.h"

namespace bl 
{

GraphicsSystem::GraphicsSystem(Engine& engine)
    : _engine(engine)
    , _instance({}, {}, true)
    , _physicalDevice(_instance.ChoosePhysicalDevice())
    , _device(_instance, _physicalDevice)
    , _window()
{

    auto displays = Display::getDisplays();
    _window = std::make_shared<RenderWindow>(_instance, displays[0]->getDesktopMode(), "Maginvox");

    _descriptorCache = std::make_shared<DescriptorSetLayoutCache>(_device);
    _pipelineLayoutCache = std::make_shared<PipelineLayoutCache>(_device);
    _swapchain = std::make_shared<Swapchain>(_device, _window);
    _renderer = std::make_shared<Renderer>(_device, _swapchain);
}

GraphicsSystem::~GraphicsSystem() 
{
}

Renderer& GraphicsSystems::getRenderer() 
{ 
    return _renderer; 
}

Instance& GraphicsSystems::getInstance() 
{ 
    return _instance; 
}

PhysicalDevice& GraphicsSystems::getPhysicalDevice() 
{ 
    return _physicalDevice; 
}

RenderWindow& GraphicsSystems::getWindow() 
{ 
    return _window; 
}

Device& GraphicsSystems::getDevice() 
{ 
    return _device; 
}

DescriptorSetLayoutCache& GraphicsSystems::getDescriptorCache() 
{ 
    return _descriptorCache; 
}

PipelineLayoutCache& GraphicsSystems::getPipelineLayoutCache() 
{ 
    return _pipelineLayoutCache; 
}

} // namespace bl
