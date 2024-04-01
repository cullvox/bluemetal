#include "GraphicsSubsystem.h"
#include "Engine.h"
#include "Core/Print.h"

namespace bl {

GraphicsSubsystem::GraphicsSubsystem(Engine* engine)
    : _engine(engine)
{
}

GraphicsSubsystem::~GraphicsSubsystem() { }

bool GraphicsSubsystem::init(const GraphicsSubsystemInitInfo* pInfo)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        blError("Could not initialize SDL 2: {}", SDL_GetError());
    }

    GfxInstance::CreateInfo createInfo{
            bl::engineVersion,
            "Maginvox",
            true
        };
    _instance = std::make_shared<GfxInstance>(createInfo);

    auto displays = Display::getDisplays();
    Window::CreateInfo windowCreateInfo{_instance, displays[0]->getDesktopMode()};
    _window = std::make_shared<Window>(windowCreateInfo);

    auto physicalDevices = _instance->getPhysicalDevices();
    _physicalDevice = pInfo && pInfo->physicalDeviceIndex
        ? physicalDevices[pInfo->physicalDeviceIndex.value()]
        : physicalDevices[0];

    GfxDevice::CreateInfo deviceCreateInfo = {_instance, _physicalDevice, _window};
    _device = std::make_shared<GfxDevice>(deviceCreateInfo);

    GfxSwapchain::CreateInfo swapchainCreateInfo = {_window};
    _swapchain = std::make_shared<GfxSwapchain>(_device, swapchainCreateInfo);

    Renderer::CreateInfo rendererCreateInfo = {_device, _swapchain};
    _renderer = std::make_shared<Renderer>(rendererCreateInfo);

    return true;
}

std::shared_ptr<Renderer> GraphicsSubsystem::getRenderer() { return _renderer; }
std::shared_ptr<GfxInstance> GraphicsSubsystem::getInstance() { return _instance; }
std::shared_ptr<GfxPhysicalDevice> GraphicsSubsystem::getPhysicalDevice() { return _physicalDevice; }
std::shared_ptr<Window> GraphicsSubsystem::getWindow() { return _window; }
std::shared_ptr<GfxDevice> GraphicsSubsystem::getDevice() { return _device; }

} // namespace bl
