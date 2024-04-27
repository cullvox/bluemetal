#include "Engine/Engine.h"
#include "Core/Print.h"

namespace bl 
{

Engine::Engine()
    : _config(std::ifstream{"Config/Config.json"})
    , _sdl()
{
    blInfo("Constructing bluemetal engine {}", bl::to_string(bl::engineVersion));

    _resourceManager = std::make_shared<ResourceManager>();
    _graphics = std::make_shared<GraphicsSystem>();
    _imgui = std::make_shared<ImGuiSubsystem>();
    _audio = std::make_shared<AudioSystem>();
}

Engine::~Engine() 
{
}

std::shared_ptr<GraphicsSystem> Engine::GetGraphics() 
{
    return graphics;
}

std::shared_ptr<AudioSystem> Engine::GetAudio() 
{
    return audio;
}

std::shared_ptr<ImGuiSystem> Engine::GetImGui() 
{
    return imgui;
}

} // namespace bl
