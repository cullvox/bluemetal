#include "Engine/Engine.h"
#include "Core/Print.h"

namespace bl {

Engine::Engine()
{
    SDL_SetMainReady();

    blInfo("Constructing bluemetal engine {}", bl::to_string(bl::engineVersion));

    // m_config = Noodle::parseFromFile("Save/Config/config.noodle");
    audio = std::make_shared<AudioSubsystem>(*this);
    graphics = std::make_shared<GraphicsSubsystem>(*this);
    imgui = std::make_shared<ImGuiSubsystem>(*this);
}

Engine::~Engine() { }

std::shared_ptr<GraphicsSubsystem> Engine::getGraphics() { return graphics; }
std::shared_ptr<AudioSubsystem> Engine::getAudio() { return audio; }
std::shared_ptr<ImGuiSubsystem> Engine::getImGui() { return imgui; }

} // namespace bl
