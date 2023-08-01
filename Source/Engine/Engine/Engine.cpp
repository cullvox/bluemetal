#include "Core/Log.h"
#include "Noodle/Noodle.h"
#include "Engine/Engine.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_bluemetal.h"

namespace bl
{

Engine& Engine::get()
{
    static Engine singleton;

    return singleton;
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

    m_renderer = std::make_shared<Renderer>(m_device, m_swapchain);

    ImGui_ImplBluemetal_InitInfo info = {};
    info.instance = m_instance;
    info.physicalDevice = m_physicalDevice;
    info.device = m_device;
    info.renderPass = m_renderer->getImGuiPass();
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