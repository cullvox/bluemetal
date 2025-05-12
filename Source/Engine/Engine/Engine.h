#pragma once

#include "Precompiled.h"
#include "Core/Flags.h"
#include "Resource/ResourceManager.h"
#include "SDLInitializer.h"
#include "Audio/AudioSystem.h"
#include "Graphics/GraphicsSystem.h"
#include "ImGui/ImGuiSystem.h"

namespace bl
{

class Engine
{
public:
    Engine(); /** @brief Constructor */
    ~Engine(); /** @brief Destructor */

    ResourceManager* GetResourceManager();
    GraphicsSystem* GetGraphics();
    ImGuiSystem* GetImGui();
    AudioSystem* GetAudio();

    Window* GetWindow();
    Renderer* GetRenderer();

private:
    nlohmann::json _config;
    SDLInitializer _sdl;
    std::unique_ptr<ResourceManager> _resourceManager;
    std::unique_ptr<GraphicsSystem> _graphics;
    std::unique_ptr<ImGuiSystem> _imgui;
    std::unique_ptr<AudioSystem> _audio;
    std::unique_ptr<Window> _window;
    std::unique_ptr<Renderer> _renderer;
};

} // namespace bl
