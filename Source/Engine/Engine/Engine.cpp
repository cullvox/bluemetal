#include "argparse/argparse.hpp"

#include "EngineVars.h"
#include "Audio/AudioSystem.h"
#include "Social/Discord.h"
#include "Core/FrameCounter.h"
#include "Core/Print.h"
#include "Editor/Editor.h"
#include "Engine/Engine.h"
#include "Engine/SDL.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanWindow.h"
#include "ImGui/ImGuiSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Scene/SceneSystem.h"
#include "Resources/ResourceSystem.h"
#include "Window/Input.h"

#include "Jolt/Core/Core.h"

namespace bl {

Engine::Engine(int argc, const char** argv)
{


    Print::Raw(fmt::fg(fmt::color::sky_blue), " 888888ba  dP                   8888ba.88ba             dP            dP \n");
    Print::Raw(fmt::fg(fmt::color::sky_blue), " 88    `8b 88                   88  `8b  `8b            88            88 \n");
    Print::Raw(fmt::fg(fmt::color::sky_blue), "a88aaaa8P' 88 dP    dP .d8888b. 88   88   88 .d8888b. d8888P .d8888b. 88 \n");
    Print::Raw(fmt::fg(fmt::color::sky_blue), " 88   `8b. 88 88    88 88ooood8 88   88   88 88ooood8   88   88'  `88 88 \n");
    Print::Raw(fmt::fg(fmt::color::sky_blue), " 88    .88 88 88.  .88 88.  ... 88   88   88 88.  ...   88   88.  .88 88 \n");
    Print::Raw(fmt::fg(fmt::color::sky_blue), " 88888888P dP `88888P' `88888P' dP   dP   dP `88888P'   dP   `88888P8 dP \n");
    Print::Raw(fmt::fg(fmt::color::sky_blue), "Engine\n");
    Print::NewLine();
    Print::Raw(fmt::fg(fmt::color::orange), "Written By: {}", "Caden (cullvox) Miller\n");
    Print::Raw(fmt::fg(fmt::color::orange), "Version:    {}\n", bl::engineVersion.ToString());
    Print::Raw(fmt::fg(fmt::color::orange), ("Build Date: {}\n", __DATE__ " " __TIME__));
    Print::NewLine();
    Print::Raw("Using:\n");
    Print::Raw("    SDL " BL_STRINGIFY(SDL_MAJOR_VERSION) "." BL_STRINGIFY(SDL_MINOR_VERSION) "\n");
    Print::Raw("    Vulkan Header " BL_STRINGIFY(VK_HEADER_VERSION) "\n");
    Print::Raw("    Jolt Physics " BL_STRINGIFY(JPH_VERSION_MAJOR) "." BL_STRINGIFY(JPH_VERSION_MINOR) "." BL_STRINGIFY(JPH_VERSION_PATCH) "\n");
    Print::Raw("    FMOD {}.{:0>2}.{:0>2}\n", (FMOD_VERSION >> 16) & 0xFFFF, (FMOD_VERSION >> 8) & 0xFF, FMOD_VERSION & 0xFF);
    Print::NewLine();
    Print::NewLine();


    argparse::ArgumentParser program("BlueMetal Engine");

    program.add_argument("-v", "--verbose")
        .help("Enable verbose logging")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        Print::Error("Argument parsing error: {}", err.what());
        Print::Info("Use --help to see available options.");
        throw;
    }

    bool verbose = program.get<bool>("--verbose");
    Print::EnableVerboseLogging(verbose);

    uint32_t flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS;

    if (SDL_Init(flags) == false) {
        throw std::runtime_error("Could not initialize SDL!");
    }

    _vars = std::make_unique<EngineVars>();
    _resourceManager = std::make_unique<ResourceSystem>(*this);
    _audio = std::make_unique<AudioSystem>(*this);
    _counter = std::make_unique<FrameCounter>();
    _graphics = std::make_unique<GraphicsSystem>(*this);
    _input = std::make_unique<InputSystem>(*this);
    _imgui = std::make_unique<ImGuiSystem>(*this, _graphics->GetWindow(), _graphics->GetRenderer());
    _physics = std::make_unique<PhysicsSystem>(*this);
    _scenes = std::make_unique<SceneSystem>(*this);
    _editor = std::make_unique<Editor>(*this);
    _discord = std::make_unique<DiscordSystem>(*this);

    auto _pointMaterial = _resourceManager->Load<Material>("Resources/Materials/DebugPoint.mat");
    auto _lineMaterial = _resourceManager->Load<Material>("Resources/Materials/DebugLine.mat");
    auto _triangleMaterial = _resourceManager->Load<Material>("Resources/Materials/DebugTriangle.mat");
    _graphics->GetRenderer()->SetDebugMaterialInstance(_pointMaterial.lock()->GetVulkanMaterial(), _lineMaterial.lock()->GetVulkanMaterial(), _triangleMaterial.lock()->GetVulkanMaterial());
}

Engine::~Engine()
{
    SDL_Quit();
    Print::Info("Shutting down BlueMetal...");
}

EngineVars& Engine::GetVars()
{
    return *_vars.get();
}

FrameCounter& Engine::GetFrameCounter()
{
    return *(_counter.get());
}

ResourceSystem* Engine::GetResourceSystem()
{
    return _resourceManager.get();
}

GraphicsSystem& Engine::GetGraphics()
{
    return *_graphics.get();
}

AudioSystem* Engine::GetAudio()
{
    return _audio.get();
}

InputSystem* Engine::GetInput()
{
    return _input.get();
}

ImGuiSystem* Engine::GetImGui()
{
    return _imgui.get();
}

Window* Engine::GetWindow()
{
    return _graphics->GetWindow();
}

Renderer* Engine::GetRenderer()
{
    return _graphics->GetRenderer();
}

PhysicsSystem& Engine::GetPhysics()
{
    return *_physics.get();
}

SceneSystem* Engine::GetSceneSystem()
{
    return _scenes.get();
}

Editor& Engine::GetEditor()
{
    return *_editor.get();
}

DiscordSystem& Engine::GetDiscord()
{
    return *_discord.get();
}

} // namespace bl
