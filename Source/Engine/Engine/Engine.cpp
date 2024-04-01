#include "Engine/Engine.h"
#include "Core/Print.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_bluemetal.h"
#include "imgui/imgui_impl_sdl2.h"

namespace bl {

Engine::Engine()
{
    SDL_SetMainReady();

    // m_config = Noodle::parseFromFile("Save/Config/config.noodle");
    audio = std::make_shared<AudioSubsystem>(this);
    graphics = std::make_shared<GraphicsSubsystem>(this);
}

Engine::~Engine() { }

void Engine::init(SubsystemFlags flags, const SubsystemInitInfo* pInfo)
{
    if (flags & SubsystemFlagBits::eSubsystemAudioBit) {
        audio->init();
    }

    if (flags & SubsystemFlagBits::eSubsystemGraphicsBit) {
        graphics->init(pInfo ? pInfo->pGraphicsInit : nullptr);
    }

    if (flags & SubsystemFlagBits::eSubsystemImGuiBit) { }
}

GraphicsSubsystem* Engine::getGraphics() { return graphics.get(); }

AudioSubsystem* Engine::getAudio() { return audio.get(); }

} // namespace bl
