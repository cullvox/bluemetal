#include "GraphicsSubsystem.h"
#include "Engine.h"
#include "Core/Print.h"

namespace bl {

GraphicsSubsystem::GraphicsSubsystem(Engine& engine)
    : _engine(engine)
{
    if (enableRenderDoc)
        _renderDoc = std::make_shared<RenderDoc>();

    _instance = std::make_shared<GfxInstance>(bl::engineVersion, "Maginvox", true);

    auto displays = Display::getDisplays();
    _window = std::make_shared<RenderWindow>(_instance, displays[0]->getDesktopMode(), "Maginvox");

    auto physicalDevices = _instance->getPhysicalDevices();

    auto it = std::find_if(physicalDevices.begin(), physicalDevices.end(), [](auto&& pd){ return pd->getType() == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; });
    
    if (it != physicalDevices.end()) _physicalDevice = *it;
    else _physicalDevice = physicalDevices[0];

    _device = std::make_shared<GfxDevice>(_instance, _physicalDevice);
    _descriptorCache = std::make_shared<DescriptorSetLayoutCache>(_device);
    _pipelineLayoutCache = std::make_shared<PipelineLayoutCache>(_device);
    _swapchain = std::make_shared<Swapchain>(_device, _window, VK_PRESENT_MODE_MAILBOX_KHR);
    _renderer = std::make_shared<Renderer>(_device, _swapchain);
}

GraphicsSubsystem::~GraphicsSubsystem() {}

std::shared_ptr<RenderDoc> GraphicsSubsystem::getRenderDoc() { return _renderDoc; }
std::shared_ptr<Renderer> GraphicsSubsystem::getRenderer() { return _renderer; }
std::shared_ptr<GfxInstance> GraphicsSubsystem::getInstance() { return _instance; }
std::vector<std::shared_ptr<GfxPhysicalDevice>> GraphicsSubsystem::getPhysicalDevices() { return _instance->getPhysicalDevices(); }
std::shared_ptr<GfxPhysicalDevice> GraphicsSubsystem::getPhysicalDevice() { return _physicalDevice; }
std::shared_ptr<RenderWindow> GraphicsSubsystem::getWindow() { return _window; }
std::shared_ptr<Swapchain> GraphicsSubsystem::getSwapchain() { return _swapchain; }
std::shared_ptr<GfxDevice> GraphicsSubsystem::getDevice() { return _device; }
std::shared_ptr<DescriptorSetLayoutCache> GraphicsSubsystem::getDescriptorCache() { return _descriptorCache; }
std::shared_ptr<PipelineLayoutCache> GraphicsSubsystem::getPipelineLayoutCache() { return _pipelineLayoutCache; }

} // namespace bl
