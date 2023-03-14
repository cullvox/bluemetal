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

    m_window = std::make_shared<Window>(displays[0].getDesktopMode(), applicationName, std::nullopt);

    if (not m_window->good())
    {
        Logger::Error("Could not create the window!");
        return;
    }



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

} // namespace bl