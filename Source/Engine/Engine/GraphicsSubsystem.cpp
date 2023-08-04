#include "GraphicsSubsystem.h"


namespace bl
{

GraphicsSubsystem::GraphicsSubsystem(Engine& engine)
    : m_engine(engine)
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

    m_instance = std::make_shared<Instance>();

    auto displays = Display::getDisplays();
    m_window = std::make_shared<Window>(m_instance, displays[0].getDesktopMode());

    auto physicalDevices = m_instance->getPhysicalDevices();
    m_physicalDevice = pInfo && pInfo->physicalDeviceIndex ? physicalDevices[pInfo->physicalDeviceIndex.value()] : physicalDevices[0];

    m_device = std::make_shared<Device>(m_instance, m_physicalDevice, m_window);
    m_swapchain = std::make_shared<Swapchain>(m_device, m_window);
    m_renderer = std::make_shared<Renderer>(m_device, m_swapchain);

}

} // namespace bl