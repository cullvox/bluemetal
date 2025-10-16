#include "Engine/Engine.h"
#include "Core/Print.h"
#include "Graphics/Renderer.h"
#include "Math/Rect.h"

namespace bl
{

std::unique_ptr<Engine> Engine::_engine = nullptr;

Engine::Engine()
    : _sdl()
{
    _engine = std::unique_ptr<Engine>(this);
    bl::Print::Info("Initializing BlueMetal v{}", bl::to_string(bl::engineVersion));

    _resourceManager = std::make_unique<ResourceManager>();
    _audio = std::make_unique<AudioSystem>();
    _graphics = std::make_unique<GraphicsSystem>();
    _imgui = std::make_unique<ImGuiSystem>(_graphics->GetWindow(), _graphics->GetRenderer());
}

Engine::~Engine() 
{
}

void Initialize()
{
    
}

void Shutdown();

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
    return _graphics->GetWindow();
}

Renderer* Engine::GetRenderer()
{
    return _graphics->GetRenderer();
}

Engine* GetEngine()
{
    return Engine::GetEngine();
}

} // namespace bl
