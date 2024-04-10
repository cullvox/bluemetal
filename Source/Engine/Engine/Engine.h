#pragma once


#include "Export.h"
#include "Precompiled.h"
#include "SDLSubsystem.h"
#include "AudioSubsystem.h"
#include "GraphicsSubsystem.h"
#include "ImGuiSubsystem.h"
#include "Core/Flags.h"

namespace bl
{

class BLUEMETAL_API Engine
{
public:

    /** @brief Constructor */
    Engine();

    /** @brief Destructor */
    ~Engine();

    std::shared_ptr<GraphicsSubsystem> getGraphics();
    std::shared_ptr<ImGuiSubsystem> getImGui();
    std::shared_ptr<AudioSubsystem> getAudio();

private:
    std::shared_ptr<SDLSubsystem> sdl;
    std::shared_ptr<GraphicsSubsystem> graphics;
    std::shared_ptr<ImGuiSubsystem> imgui;
    std::shared_ptr<AudioSubsystem> audio;
};

} // namespace bl
