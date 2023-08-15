#include "GraphicsSubsystem.h"

#include "Engine.h"

namespace bl
{

GraphicsSubsystem::GraphicsSubsystem(Engine* pEngine)
    : m_pEngine(pEngine)
{
}

GraphicsSubsystem::~GraphicsSubsystem()
{
}

void GraphicsSubsystem::init(const GraphicsSubsystemInitInfo* pInfo)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    GraphicsInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.applicationName =

    m_instance = std::make_unique<GraphicsInstance>();

    auto displays = Display::getDisplays();
    m_window = std::make_unique<Window>(m_instance.get(), displays[0].getDesktopMode());

    auto physicalDevices = m_instance->getPhysicalDevices();
    m_pPhysicalDevice = pInfo && pInfo->physicalDeviceIndex ? physicalDevices[pInfo->physicalDeviceIndex.value()] : physicalDevices[0];


    GraphicsDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.pInstance = m_instance.get();
    deviceCreateInfo.pPhysicalDevice = m_pPhysicalDevice;
    deviceCreateInfo.pWindow = m_window.get(); 

    m_device = std::make_unique<GraphicsDevice>(deviceCreateInfo);
    m_swapchain = std::make_unique<Swapchain>(m_device.get(), m_window.get());
    m_renderer = std::make_unique<Renderer>(m_device.get(), m_swapchain.get());

}

Renderer* GraphicsSubsystem::getRenderer()
{
    return m_renderer.get();
}

} // namespace bl