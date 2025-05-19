#include "Engine/Engine.h"
#include "Core/Print.h"
#include "Graphics/Renderer.h"
#include "Math/Rect.h"
#include <memory>

namespace bl 
{

Engine::Engine()
    : _sdl()
{
    blInfo("Constructing bluemetal engine {}", bl::to_string(bl::engineVersion));

    _resourceManager = std::make_unique<ResourceManager>();
    _audio = std::make_unique<AudioSystem>(this);
    _graphics = std::make_unique<GraphicsSystem>(this);

    auto displays = Display::GetDisplays();

    _graphics->GetDevice()->WaitForDevice();
    _window = _graphics->CreateWindow("Maginvox", Rect2D{{}, displays[0].GetDesktopMode().extent}, false);

    _renderer = _graphics->CreateRenderer(_window.get());

    auto vulkanWindow = dynamic_cast<VulkanWindow*>(_window.get());
    _imgui = std::make_unique<ImGuiSystem>(this, vulkanWindow, _renderer.get());

    _resourceManager->RegisterBuilder({"Shader", "Texture", "Model"}, _graphics.get());
    _resourceManager->RegisterBuilder({"Audio"}, _audio.get());
}

Engine::~Engine() 
{
    std::ignore = _resourceManager.release();
    std::ignore = _renderer.release();
    std::ignore = _imgui.release();
    std::ignore = _window.release();
    std::ignore = _graphics.release();
    std::ignore = _audio.release();
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
