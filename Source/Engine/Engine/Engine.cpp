#include "Engine/Engine.h"
#include "Core/Print.h"
#include "Noodle/Noodle.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_bluemetal.h"
#include "imgui/imgui_impl_sdl2.h"

namespace bl {

Engine::Engine()
    : graphics(std::make_unique<GraphicsSubsystem>(this))
    , audio(std::make_unique<AudioSubsystem>(this))
{
    blInfo("using bluemetal {}", engineVersion);
    blInfo(""
    SDL_SetMainReady();

    const std::string defaultConfig =
#include "default.nwdl"
        ;

    // m_config = Noodle::parseFromFile("Save/Config/config.noodle");
}

Engine::~Engine() { }

void Engine::init(SubsystemFlags flags, const SubsystemInitInfo* pInfo)
{
    if (flags & eSubsystemAudioBit) {
        audio->init();
    }

    if (flags & eSubsystemGraphicsBit) {
        graphics->init(pInfo ? pInfo->pGraphicsInit : nullptr);
    }

    if (flags & eSubsystemImGuiBit) { }
}

GraphicsSubsystem* Engine::getGraphics() { return graphics.get(); }

AudioSubsystem* Engine::getAudio() { return audio.get(); }

} // namespace bl
