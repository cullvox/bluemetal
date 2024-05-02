#include "Engine/Engine.h"
#include "Core/Print.h"

namespace bl 
{

Engine::Engine()
    : _sdl()
{
    blInfo("Constructing bluemetal engine {}", bl::to_string(bl::engineVersion));

    _resourceManager = std::make_unique<ResourceManager>(this);
    _graphics = std::make_unique<GraphicsSystem>(this);
    _window = _graphics->CreateWindow();
    _renderer = _graphics->CreateRenderer(_window);
    _imgui = std::make_unique<ImGuiSystem>(this);
    _audio = std::make_unique<AudioSystem>(this);
}

Engine::~Engine() 
{
}

std::shared_ptr<GraphicsSystem> Engine::GetGraphics() 
{
    return _graphics.get();
}

std::shared_ptr<AudioSystem> Engine::GetAudio() 
{
    return _audio.get();
}

std::shared_ptr<ImGuiSystem> Engine::GetImGui() 
{
    return _imgui.get();
}

} // namespace bl
