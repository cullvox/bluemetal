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
    _window = _graphics->CreateWindow("Maginvox", Rect2D{{}, displays[0].GetDesktopMode().extent}, false);
    _renderer = std::make_unique<Renderer>(_window.get());
    _imgui = std::make_unique<ImGuiSystem>(this, _window.get(), _renderer.get());

    _resourceManager->RegisterBuilder({"Shader", "Texture"}, _graphics.get());
    _resourceManager->RegisterBuilder({"Sound"}, _audio.get());
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
