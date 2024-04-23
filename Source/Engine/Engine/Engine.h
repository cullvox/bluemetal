#pragma once


#include "Export.h"
#include "Precompiled.h"

#include "Resource/ResourceManager.h"

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

    std::shared_ptr<GraphicsSubsystem> getGraphics();
    std::shared_ptr<ImGuiSubsystem> getImGui();
    std::shared_ptr<AudioSystem> getAudio();

private:
    std::shared_ptr<ResourceManager> resourceManager;
    std::shared_ptr<SDLSubsystem> sdl;
    std::shared_ptr<GraphicsSubsystem> graphics;
    std::shared_ptr<ImGuiSubsystem> imgui;
    std::shared_ptr<AudioSystem> audio;
};

} // namespace bl
