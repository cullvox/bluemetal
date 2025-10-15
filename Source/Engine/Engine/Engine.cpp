#include "Engine/Engine.h"
#include "Core/Print.h"
#include "Graphics/Renderer.h"
#include "Math/Rect.h"

namespace bl
{

Engine::Engine()
    : _sdl()
{
    bl::Print::Info("Initializing BlueMetal v{}", bl::to_string(bl::engineVersion));

    _resourceManager = std::make_unique<ResourceManager>();
    _audio = std::make_unique<AudioSystem>();
    _graphics = std::make_unique<GraphicsSystem>();
    _imgui = std::make_unique<ImGuiSystem>(_graphics->GetWindow(), _graphics->GetRenderer());
}

Engine::~Engine() 
{
}

Engine* Engine::GetEngine()
{
    static std::unique_ptr<Engine> engine = std::unique_ptr<Engine>(new Engine());
    return engine.get();
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
