#include "Engine/Engine.h"
#include "Core/Print.h"
#include "Graphics/Renderer.h"
#include "Math/Rect.h"

namespace bl
{

Engine::Engine()
    : _sdl()
{
}

Engine::~Engine() 
{
}

void Engine::Initialize()
{
    bl::Print::Info("Initializing BlueMetal v{}", bl::to_string(bl::engineVersion));

    _resourceManager = std::make_unique<ResourceSystem>(*this);
    _audio = std::make_unique<AudioSystem>(*this);
    _graphics = std::make_unique<GraphicsSystem>(*this);
    _imgui = std::make_unique<ImGuiSystem>(*this, _graphics->GetWindow(), _graphics->GetRenderer());
}

void Engine::Shutdown()
{
    bl::Print::Info("Shutting down BlueMetal...");
    _imgui.reset();
    _graphics.reset();
    _audio.reset();
    _resourceManager.reset();
}

ResourceSystem* Engine::GetResourceManager()
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

} // namespace bl
