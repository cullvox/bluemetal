#include "Core/Log.hpp"
#include "Engine/Engine.hpp"

namespace bl
{

Engine::Engine(const std::string& applicationName) noexcept
    : m_window()
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


/*
    Window window{ displays[0].getDesktopMode() };
    RenderDevice renderDevice{ window };
    Swapchain swapchain{ window, renderDevice };

    if (not swapchain.good())
        return 1;W

    bl::Renderer renderer{ renderDevice, swapchain };
    bl::FrameCounter frameCounter{};
    bl::InputSystem inputSystem{};
    bl::InputController windowInput{};
    CharacterController characterController{};
*/

}

Engine::~Engine()
{
}

bool Engine::run() noexcept
{
    
    while (not m_close)
    {

        m_renderer.beginFrame();


        m_renderer.endFrame();
    }

    return true;
}

} // namespace bl