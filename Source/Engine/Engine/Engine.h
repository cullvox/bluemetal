#pragma once

#include "Precompiled.h"
#include "Core/Flags.h"
#include "Resources/ResourceManager.h"
#include "SDLInitializer.h"
#include "Audio/AudioSystem.h"
#include "Graphics/GraphicsSystem.h"
#include "ImGui/ImGuiSystem.h"

namespace bl
{

class Engine
{
    nlohmann::json _config;
    SDLInitializer _sdl;
    static std::unique_ptr<ResourceManager> _resourceManager;
    static std::unique_ptr<GraphicsSystem> _graphics;
    static std::unique_ptr<ImGuiSystem> _imgui;
    static std::unique_ptr<AudioSystem> _audio;
    static std::unique_ptr<Window> _window;
    static std::unique_ptr<Renderer> _renderer;
    static std::unique_ptr<Engine> _engine;

public:
    static Engine* 

    static ResourceManager* GetResourceManager();
    static GraphicsSystem* GetGraphics();
    static ImGuiSystem* GetImGui();
    static AudioSystem* GetAudio();
    static Window* GetWindow();
    static Renderer* GetRenderer();
};



} // namespace bl
