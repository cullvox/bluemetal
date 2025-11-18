#pragma once

#include "Precompiled.h"
#include "Core/Flags.h"
#include "Core/FrameCounter.h"
#include "Engine/SDL.h"
#include "Resources/ResourceSystem.h"
#include "Audio/AudioSystem.h"
#include "Graphics/GraphicsSystem.h"
#include "Window/Input.h"
#include "ImGui/ImGuiSystem.h"

namespace bl
{

class Engine
{
    nlohmann::json _config;
    SDLInitializer _sdl;
    FrameCounter _counter;
    std::unique_ptr<ResourceSystem> _resourceManager;
    std::unique_ptr<GraphicsSystem> _graphics;
    std::unique_ptr<ImGuiSystem> _imgui;
    std::unique_ptr<AudioSystem> _audio;
    std::unique_ptr<InputSystem> _input;
    std::unique_ptr<Window> _window;
    std::unique_ptr<Renderer> _renderer;

public:
    Engine();
    ~Engine();

    void Initialize();
    void Shutdown();

    FrameCounter& GetFrameCounter();
    ResourceSystem* GetResourceManager();
    GraphicsSystem* GetGraphics();
    InputSystem* GetInput();
    ImGuiSystem* GetImGui();
    AudioSystem* GetAudio();
    Window* GetWindow();
    Renderer* GetRenderer();
};



} // namespace bl
