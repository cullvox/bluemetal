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
    audio = std::make_shared<AudioSubsystem>(*this);
    graphics = std::make_shared<GraphicsSubsystem>(*this);
}

Engine::~Engine() { }

std::shared_ptr<GraphicsSubsystem> Engine::getGraphics() { return graphics; }
std::shared_ptr<AudioSubsystem> Engine::getAudio() { return audio; }

} // namespace bl
