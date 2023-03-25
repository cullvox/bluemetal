#include "Core/Log.hpp"
#include "Engine/Engine.hpp"

#include "imgui_impl_bloodlust.h"

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
    m_inputSystem.registerAxis("Walk", { {Key::KeyW, 1.0f}, {Key::KeyS, -1.0f} });
    m_inputSystem.registerAction("Exit", { Key::WindowClose });
    m_inputSystem.registerAction("Resize", { Key::WindowResize });

    auto walkDelegate = m_inputSystem.getAxisDelegate("Walk");
    *walkDelegate += [&](float scale) { Logger::Log("Walking {}!", scale); };

    auto resizeDelegate = m_inputSystem.getActionDelegate("Resize");
    *resizeDelegate += [&](){ m_renderer.recreate(); };

    auto exitDelegate = m_inputSystem.getActionDelegate("Exit");
    *exitDelegate += [&]() { m_close = true; };

    ImGui_ImplBloodLust_Init(m_window, m_inputSystem, m_renderDevice, m_renderer);
}

Engine::~Engine()
{
    ImGui_ImplBloodLust_Shutdown();
}

bool Engine::run() noexcept
{
    
    while (not m_close)
    {
        m_frameCounter.beginFrame();
        
        m_inputSystem.poll();
        
        VkCommandBuffer commandBuffer{};

        if (m_renderer.beginFrame(commandBuffer))
        {

            ImGui_ImplBloodLust_NewFrame();

            ImGui::ShowDemoWindow();

            ImGui::Render();


            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

            m_renderer.endFrame();
        }

        m_frameCounter.endFrame();
    }

    return true;
}

} // namespace bl