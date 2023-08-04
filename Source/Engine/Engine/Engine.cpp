#include "Core/Log.h"
#include "Noodle/Noodle.h"
#include "Engine/Engine.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_bluemetal.h"

namespace bl
{

Engine::Engine()
    : m_graphics(*this)
    , m_audio(*this)
{
    SDL_SetMainReady();
    
    const std::string defaultConfig = 
        #include "default.nwdl"
    ;

    m_config = Noodle::parseFromFile("Save/Config/config.noodle");

}

Engine::~Engine()
{
}

void Engine::init(SubsystemFlags flags, const SubsystemInitInfo* pInfo)
{
    if (flags & eSubsystemAudioBit)
    {
        audio->init();
    }

    if (flags & eSubsystemGraphicsBit)
    {
        graphics->init(pInfo->pGraphicsInit);
    }
}

GraphicsSubsystem* Engine::getGraphics()
{
    return graphics.get();
}

AudioSubsystem* Engine::getAudio()
{
    return audio.get();
}

} // namespace bl