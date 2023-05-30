#include "Core/Log.hpp"
#include "Engine/Engine.hpp"
#include "Render/Renderer/PresentRenderPass.hpp"
#include "ImGui/ImGui.hpp"
#include "Noodle/Noodle.hpp"

blEngine::blEngine(const std::string& applicationName)
    : _window()
    , _close(false)
    , _inputSystem()
{
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        throw std::runtime_error("Could not initialize SDL2!");
    }

    std::vector<blDisplay> displays = blDisplay::getDisplays();

    
    _window = std::make_shared<blWindow>(displays[0].getDesktopMode());
    _inputSystem = blInputSystem::getInstance();
    _renderDevice = std::make_shared<blRenderDevice>(_window);
    _swapchain = std::make_shared<blSwapchain>(_renderDevice);

    const std::string defaultConfig = 
        #include "defaultConfig.nwdl"
    ;

    blNoodle config = blNoodle::parseFromFile("Save/Config/config.noodle");
    int& value = config["window"]["width"].get<int>();
    config["window"]["height"] = 1600;
    value = value + 100;

    config.dumpToFile("Save/Config/config.noodle");

    _presentPass = std::make_shared<blPresentRenderPass>(_renderDevice, _swapchain, [&](vk::CommandBuffer cmd){
        blImGui::beginFrame();
    
        ImGui::Begin("Debug Info");
        ImGui::Text("Graphics Device: %s", _renderDevice->getDeviceName());
        ImGui::Text("Graphics Vendor: %s", _renderDevice->getVendorName());
        ImGui::Text("F/S: %d", _frameCounter.getFramesPerSecond());
        ImGui::Text("MS/F: %.2f", _frameCounter.getMillisecondsPerFrame());
        ImGui::Text("Average F/S (Over 10 Seconds): %.1f", _frameCounter.getAverageFramesPerSecond(10));
        ImGui::Text("Average MS/F (Over 144 Frames): %.2f", _frameCounter.getAverageMillisecondsPerFrame(144));
        ImGui::End();

        ImGui::Begin("Settings");
        static int presentModeSelected;
        ImGui::RadioButton("FIFO (VSync)", &presentModeSelected, 0);
        ImGui::RadioButton("Mailbox (Preferred)", &presentModeSelected, 1);
        ImGui::End();

        ImGui::ShowDemoWindow();
        
        blImGui::endFrame(cmd);
    });

    std::vector<std::shared_ptr<blRenderPass>> passes
    {
        _presentPass
    };

    _renderer = std::make_shared<blRenderer>(_renderDevice, _swapchain, passes);

    blImGui::init(_window, _renderDevice, _presentPass);
}

blEngine::~blEngine()
{
    blImGui::shutdown();
}

bool blEngine::run()
{
    
    while (not _inputSystem->shouldClose())
    {
        _frameCounter.beginFrame();
        _inputSystem->poll();
        
        _renderer->render();
        _frameCounter.endFrame();
    }

    return true;
}