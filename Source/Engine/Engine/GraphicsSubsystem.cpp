#include "GraphicsSubsystem.h"
#include "Engine.h"
#include "Core/Print.h"

namespace bl {

GraphicsSubsystem::GraphicsSubsystem(Engine& engine)
    : _engine(engine)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error("Could not initialize SDL2!");
    }

    _instance = std::make_shared<GfxInstance>(bl::engineVersion, "Maginvox", true);

    auto displays = Display::getDisplays();
    _window = std::make_shared<Window>(_instance, displays[0]->getDesktopMode(), "Maginvox");

    auto physicalDevices = _instance->getPhysicalDevices();
    _physicalDevice = physicalDevices[0];

    _device = std::make_shared<GfxDevice>(_instance, _physicalDevice);
    _swapchain = std::make_shared<GfxSwapchain>(_device, _window);
    _renderer = std::make_shared<Renderer>(_device, _swapchain);
}

GraphicsSubsystem::~GraphicsSubsystem() { }

std::shared_ptr<Renderer> GraphicsSubsystem::getRenderer() { return _renderer; }
std::shared_ptr<GfxInstance> GraphicsSubsystem::getInstance() { return _instance; }
std::shared_ptr<GfxPhysicalDevice> GraphicsSubsystem::getPhysicalDevice() { return _physicalDevice; }
std::shared_ptr<Window> GraphicsSubsystem::getWindow() { return _window; }
std::shared_ptr<GfxDevice> GraphicsSubsystem::getDevice() { return _device; }

} // namespace bl
