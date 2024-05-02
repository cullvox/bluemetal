#pragma once


#include "Export.h"
#include "Precompiled.h"

#include "Resource/ResourceManager.h"

#include "Core/Json.h"
#include "SDLInitializer.h"
#include "Audio/AudioSystem.h"
#include "Graphics/GraphicsSystem.h"
#include "ImGuiSubsystem.h"
#include "Core/Flags.h"

namespace bl
{

class Engine
{
public:
    Engine(); /** @brief Constructor */
    ~Engine(); /** @brief Destructor */

    GraphicsSystem* GetGraphics();
    ImGuiSystem* GetImGui();
    AudioSystem* GetAudio();

private:
    nlohmann::json _config;
    SDLInitializer _sdl;
    std::unique_ptr<ResourceManager> _resourceManager;
    std::unique_ptr<GraphicsSystem> _graphics;
    std::unique_ptr<ImGuiSystem> _imgui;
    std::unique_ptr<AudioSystem> _audio;
};

} // namespace bl
