#pragma once


#include "Export.h"
#include "Precompiled.h"

#include "Resource/ResourceManager.h"

#include "Core/Json.h"
#include "SDLSubsystem.h"
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

    std::shared_ptr<GraphicsSystem> GetGraphics();
    std::shared_ptr<ImGuiSystem> GetImGui();
    std::shared_ptr<AudioSystem> GetAudio();

private:
    nlohmann::json _config;
    SDLInitializer _sdl;
    std::shared_ptr<ResourceManager> _resourceManager;
    std::shared_ptr<GraphicsSystem> _graphics;
    std::shared_ptr<ImGuiSubsystem> _imgui;
    std::shared_ptr<AudioSystem> _audio;
};

} // namespace bl
