#include "Core/Log.hpp"
#include "Engine/Engine.hpp"

namespace bl
{

Engine::Engine(const std::string& applicationName) noexcept
    : m_window()
    , m_close(false)
{
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::Error("Could not initialize SDL2!");
        return;
    }

    std::vector<bl::Display> displays = bl::Display::getDisplays();

    m_window = Window{ displays[0].getDesktopMode() };
    if (not m_window.good())
    {
        Logger::Error("Could not create the window!");
        return;
    }

    m_renderDevice = RenderDevice{ m_window };
    if (not m_renderDevice.good())
    {
        Logger::Error("Could not create the render device!");
        return;
    }

    m_swapchain = Swapchain{ m_window, m_renderDevice };
    if (not m_swapchain.good())
    {
        Logger::Error("Could not create the swapchain!");
        return;
    }

    m_renderer = Renderer{ m_renderDevice, m_swapchain };
    if (not m_renderer.good())
    {
        Logger::Error("Could not create the renderer!");
        return;
    }

    m_inputSystem = InputSystem{};

}

Engine::~Engine()
{
}

bool Engine::run() noexcept
{
    
    while (not m_close)
    {
        m_frameCounter.beginFrame();
        
        m_inputSystem.poll();
        
        m_renderer.beginFrame();

        

        m_renderer.endFrame();
        m_frameCounter.endFrame();
    }

    return true;
}

} // namespace bl