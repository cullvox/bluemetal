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

void Engine::init(SubsystemFlags flags)
{
    if (flags & eSubsystemAudioBit)
    {
        m_audio.init();
    }
}

} // namespace bl