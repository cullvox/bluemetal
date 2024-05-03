#include "Engine/Engine.h"
#include "Core/Print.h"

namespace bl 
{

Engine::Engine()
    : _sdl()
{
    blInfo("Constructing bluemetal engine {}", bl::to_string(bl::engineVersion));

    _resourceManager = std::make_unique<ResourceManager>();
    _graphics = std::make_unique<GraphicsSystem>(this);

    auto displays = _graphics->GetDisplays();
    _window = _graphics->CreateWindow("Maginvox", displays[0]->GetDesktopMode(), false);
    _renderer = _graphics->CreateRenderer(_window.get());
    _imgui = std::make_unique<ImGuiSystem>(this, _renderer.get());
    _audio = std::make_unique<AudioSystem>(this);
}

Engine::~Engine() 
{
}

ResourceManager* Engine::GetResourceManager()
{
    return _resourceManager.get();
}

GraphicsSystem* Engine::GetGraphics() 
{
    return _graphics.get();
}

AudioSystem* Engine::GetAudio() 
{
    return _audio.get();
}

ImGuiSystem* Engine::GetImGui() 
{
    return _imgui.get();
}

Window* Engine::GetWindow()
{
    return _window.get();
}

Renderer* Engine::GetRenderer()
{
    return _renderer.get();
}


} // namespace bl
