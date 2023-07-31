#include "Engine/Engine.h"
#include "Core/Log.h"
#include "Render/PresentRenderPass.h"
#include "Noodle/Noodle.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_bluemetal.h"

namespace bl
{

//Helper to display a little (?) mark which shows a tooltip when hovered.
//In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

Engine::Engine(const std::string& appName)
    : m_shouldClose(false)
    , m_showImGui(false)
{
    (void)appName;

    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    std::vector<Display> displays = Display::getDisplays();

    
    const std::string defaultConfig = 
        #include "defaultConfig.nwdl"
    ;

    m_config = Noodle::parseFromFile("Save/Config/config.noodle");
    int& value = m_config["window"]["width"].get<int>();
    m_config["window"]["height"] = 1600;
    value = value + 100;

    // Noodle presentModeIndexNode = m_config["render"]["presentMode"]; 

    //_inputSystem = blInputSystem::getInstance();
    m_instance = std::make_shared<Instance>();
    m_physicalDevice = m_instance->getPhysicalDevices()[0];
    m_window = std::make_shared<Window>(m_instance, displays[0].getDesktopMode());
    m_device = std::make_shared<Device>(m_instance, m_physicalDevice, m_window);
    m_swapchain = std::make_shared<Swapchain>(m_device, m_window);


    m_config.dumpToFile("Save/Config/config.noodle");

    static std::vector<VkPresentModeKHR> presentModes = m_physicalDevice->getPresentModes(m_window);
    static std::vector<VkSurfaceFormatKHR> surfaceFormats = m_physicalDevice->getSurfaceFormats(m_window);

    m_presentPass = std::make_shared<PresentRenderPass>(m_device, m_swapchain, 
        [&](VkCommandBuffer cmd)
        {
            if (!m_showImGui) return;

            ImGui_ImplBluemetal_BeginFrame();

            auto currentPresentMode = m_swapchain->getPresentMode();
            auto currentSurfaceFormat = m_swapchain->getSurfaceFormat();

            ImGui::Begin("Debug Info");
            ImGui::Text("Graphics Device: %s", m_physicalDevice->getDeviceName().c_str());
            ImGui::Text("Graphics Vendor: %s", m_physicalDevice->getVendorName().c_str());
            ImGui::Text("F/S: %d", _frameCounter.getFramesPerSecond());
            ImGui::Text("MS/F: %.2f", _frameCounter.getMillisecondsPerFrame());
            ImGui::Text("Average F/S (Over 10 Seconds): %.1f", _frameCounter.getAverageFramesPerSecond(10));
            ImGui::Text("Average MS/F (Over 144 Frames): %.2f", _frameCounter.getAverageMillisecondsPerFrame(144));
            ImGui::Text("Present Mode: %s", string_VkPresentModeKHR(currentPresentMode));
            ImGui::Text("Surface Format: (%s, %s)", string_VkFormat(currentSurfaceFormat.format), string_VkColorSpaceKHR(currentSurfaceFormat.colorSpace));
    
            ImGui::End();

            ImGui::Begin("Advanced Configuration");

            ImGui::SeparatorText("ImGui Interface");

            static float alpha = 1.0f;
            if (ImGui::SliderFloat("UI Alpha Transparency", &alpha, 0.1f, 1.0f))
            {
                ImGui::GetStyle().Alpha = alpha;
            }

            ImGui::SeparatorText("Graphics");

            
            if (ImGui::CollapsingHeader("Surface Formats"))
            {

                static int surfaceFormatSelected = 0;
                for (size_t i = 0; i < surfaceFormats.size(); i++)
                {
                    HelpMarker("Changes to this value will require a restart to change.");
                    std::string name = fmt::format("({}, {})", string_VkFormat(surfaceFormats[i].format), string_VkColorSpaceKHR(surfaceFormats[i].colorSpace));
                    if (ImGui::RadioButton(name.c_str(), &surfaceFormatSelected, i))
                    {
                        m_config["render"]["presentMode"] = (int)i;
                    }
                }
            }

            if (ImGui::CollapsingHeader("Present Modes"))
            {
                static int selectedPresentMode = 0;
                for (size_t i = 0; i < presentModes.size(); i++)
                {
                    std::string name = fmt::format("({})", string_VkPresentModeKHR(presentModes[i]));
                    if (ImGui::RadioButton(name.c_str(), &selectedPresentMode, i))
                    {
                        static int value = 0;
                        value = i;
                        _postRenderCommands.push([&](){
                            m_swapchain->recreate(presentModes[value], m_swapchain->getSurfaceFormat());
                            m_renderer->resize(m_swapchain->getExtent());
                        });
                    }
                }
            }

            ImGui::End();

            ImGui::ShowDemoWindow();
            
            ImGui_ImplBluemetal_EndFrame(cmd);
        });

    std::vector<std::shared_ptr<RenderPass>> passes
    {
        m_presentPass
    };

    m_renderer = std::make_shared<Renderer>(m_device, m_swapchain, passes);

    ImGui_ImplBluemetal_InitInfo info = {};
    info.instance = m_instance;
    info.physicalDevice = m_physicalDevice;
    info.device = m_device;
    info.renderPass = m_presentPass;
    info.window = m_window;

    if (!ImGui_ImplBluemetal_Init(&info))
    {
        throw std::runtime_error("Could not init ImGui!");
    }
}

Engine::~Engine()
{
    ImGui_ImplBluemetal_Shutdown();
}

bool Engine::run()
{

    while (!m_shouldClose)
    {
        _frameCounter.beginFrame();
        //_inputSystem->poll();

        const Uint8* state = SDL_GetKeyboardState(nullptr);

        if (state[SDL_SCANCODE_F3])
            m_showImGui = !m_showImGui;
        
        if (state[SDL_SCANCODE_ESCAPE])
            m_shouldClose = true;

        SDL_Event event = {};
        while(SDL_PollEvent(&event))
        {
            if (m_showImGui)
                ImGui_ImplSDL2_ProcessEvent(&event);
        }




        m_renderer->render();
        _frameCounter.endFrame();
    
        for (size_t i = 0; i < _postRenderCommands.size(); i++)
        {
            _postRenderCommands.front()();
            _postRenderCommands.pop();
        }
    }

    m_config.dumpToFile("Save/Config/config.noodle");

    return true;
}

} // namespace bl