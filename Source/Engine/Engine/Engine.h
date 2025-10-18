#pragma once

#include "Precompiled.h"
#include "Core/Flags.h"
#include "Engine/SDL.h"
#include "Resources/ResourceSystem.h"
#include "Audio/AudioSystem.h"
#include "Graphics/GraphicsSystem.h"
#include "ImGui/ImGuiSystem.h"

namespace bl
{

class Engine
{
    nlohmann::json _config;
    SDLInitializer _sdl;
    std::unique_ptr<ResourceSystem> _resourceManager;
    std::unique_ptr<GraphicsSystem> _graphics;
    std::unique_ptr<ImGuiSystem> _imgui;
    std::unique_ptr<AudioSystem> _audio;
    std::unique_ptr<Window> _window;
    std::unique_ptr<Renderer> _renderer;

public:
    Engine();
    ~Engine();

    void Initialize();
    void Shutdown();

    ResourceSystem* GetResourceManager();
    GraphicsSystem* GetGraphics();
    ImGuiSystem* GetImGui();
    AudioSystem* GetAudio();
    Window* GetWindow();
    Renderer* GetRenderer();
};



} // namespace bl
