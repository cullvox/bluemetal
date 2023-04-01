#include "Core/Log.hpp"
#include "Engine/Engine.hpp"

#include "ImGui.hpp"

namespace bl
{

void applyFont()
{
    ImFontConfig cfg;
    cfg.OversampleH = 5;
    ImFont* pFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Regular.ttf", 13.0f);
    ImGui::PushFont(pFont);
}

void applyImStyle()
{
    //float SCALE = 2.0f;
    //ImFontConfig cfg;
    //cfg.SizePixels = 13 * SCALE;
    //ImGui::GetIO().Fonts->AddFontDefault(&cfg);



    ::ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    //style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    //style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.GrabRounding = style.FrameRounding = 2.3f;
}

Engine::Engine(const std::string& applicationName) noexcept
    : m_window()
    , m_close(false)
    , m_inputSystem()
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

    m_inputSystem.registerAxis("Walk", { {Key::KeyW, 1.0f}, {Key::KeyS, -1.0f} });
    m_inputSystem.registerAction("Exit", { Key::WindowClose });
    m_inputSystem.registerAction("Resize", { Key::WindowResize });

    auto resizeDelegate = m_inputSystem.addActionCallback("Resize", [&]() {
        m_renderer.recreate();
    });

    auto exitHandle = m_inputSystem.addActionCallback("Exit", [&]() { m_close = true; });

    ImmediateModeGUI::init(m_window, m_inputSystem, m_renderDevice, m_renderer);

    applyImStyle();
    
}

Engine::~Engine()
{
    ImmediateModeGUI::shutdown();
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
            ImmediateModeGUI::beginFrame();

            ImGui::ShowDemoWindow();


            ImmediateModeGUI::endFrame(commandBuffer);
            m_renderer.endFrame();
        }

        m_frameCounter.endFrame();
    }

    return true;
}

} // namespace bl