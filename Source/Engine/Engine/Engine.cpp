#include "Engine/Engine.h"
#include "Core/Log.h"
#include "Render/PresentRenderPass.h"
#include "Noodle/Noodle.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_bluemetal.h"

namespace bl
{

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
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
    : _close(false)
{
    (void)appName;

    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    std::vector<Display> displays = Display::getDisplays();

    
    
    //_inputSystem = blInputSystem::getInstance();
    m_instance = std::make_shared<Instance>();
    m_physicalDevice = m_instance->getPhysicalDevices()[0];
    m_window = std::make_shared<Window>(m_instance, displays[0].getDesktopMode());
    m_device = std::make_shared<Device>(m_instance, m_physicalDevice, m_window);
    m_swapchain = std::make_shared<Swapchain>(m_device, m_window);

    const std::string defaultConfig = 
        #include "defaultConfig.nwdl"
    ;

    Noodle config = Noodle::parseFromFile("Save/Config/config.noodle");
    int& value = config["window"]["width"].get<int>();
    config["window"]["height"] = 1600;
    value = value + 100;

    config.dumpToFile("Save/Config/config.noodle");

    m_presentPass = std::make_shared<PresentRenderPass>(m_device, m_swapchain, 
    [&](VkCommandBuffer cmd)
    {
        ImGui_ImplBluemetal_BeginFrame();

        ImGui::Begin("Debug Info");
        ImGui::Text("Graphics Device: %s", m_physicalDevice->getDeviceName().c_str());
        ImGui::Text("Graphics Vendor: %s", m_physicalDevice->getVendorName().c_str());
        ImGui::Text("F/S: %d", _frameCounter.getFramesPerSecond());
        ImGui::Text("MS/F: %.2f", _frameCounter.getMillisecondsPerFrame());
        ImGui::Text("Average F/S (Over 10 Seconds): %.1f", _frameCounter.getAverageFramesPerSecond(10));
        ImGui::Text("Average MS/F (Over 144 Frames): %.2f", _frameCounter.getAverageMillisecondsPerFrame(144));
        ImGui::End();

        ImGui::Begin("Configuration");
        static int presentModeSelected;

        ImGui::SeparatorText("Interface");

        static float alpha = 1.0f;
        if (ImGui::SliderFloat("UI Alpha Transparency", &alpha, 0.1f, 1.0f))
        {
            ImGui::GetStyle().Alpha = alpha;
        }

        ImGui::SeparatorText("Swapchain Options");
        
        if (ImGui::RadioButton("FIFO", &presentModeSelected, 0))
        {
            _postRenderCommands.push([&](){
                m_swapchain->recreate(VK_PRESENT_MODE_FIFO_KHR);
                m_renderer->resize(m_swapchain->getExtent());
            });
        }
        ImGui::SameLine();
        HelpMarker("Normal VSync, use Mailbox if your system supports it.");
        
        ImGui::BeginDisabled(!m_swapchain->isMailboxSupported());
        if (ImGui::RadioButton("Mailbox", &presentModeSelected, 1))
        {      
            _postRenderCommands.push([&](){
                m_swapchain->recreate(VK_PRESENT_MODE_MAILBOX_KHR);
                m_renderer->resize(m_swapchain->getExtent());
            }); 
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        HelpMarker( (!m_swapchain->isMailboxSupported()) ? "Mailbox is not supported on this system." : "Performs fast like Immediate but with no tearing, like FIFO.");

        ImGui::BeginDisabled(!m_swapchain->isImmediateSupported());        
        if (ImGui::RadioButton("Immediate", &presentModeSelected, 2))
        {            
            _postRenderCommands.push([&](){
                m_swapchain->recreate(VK_PRESENT_MODE_IMMEDIATE_KHR);
                m_renderer->resize(m_swapchain->getExtent());
            }); 
            
        }
        ImGui::EndDisabled();
        
        ImGui::SameLine();
        HelpMarker( (!m_swapchain->isImmediateSupported()) ?  "Immediate is not supported on this system." : "Renders as fast as possible to screen, may cause screen tearing.");

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
    
    //while (!_inputSystem->shouldClose())
    //{
    //    _frameCounter.beginFrame();
    //    _inputSystem->poll();
    //    
    //    _renderer->render();
    //    _frameCounter.endFrame();
    //
    //    for (size_t i = 0; i < _postRenderCommands.size(); i++)
    //    {
    //        _postRenderCommands.front()();
    //        _postRenderCommands.pop();
    //    }
    //}

    return true;
}

} // namespace bl