#include "argparse/argparse.hpp"

#include "Audio/AudioSystem.h"
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
#include "Resources/ResourceSystem.h"
#include "Window/Input.h"

namespace bl {

Engine::Engine(int argc, const char** argv)
{
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

    Print::Info("Initializing BlueMetal v{}", bl::engineVersion.ToString());

    uint32_t flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS;

    if (SDL_Init(flags) == false) {
        throw std::runtime_error("Could not initialize SDL!");
    }

    _resourceManager = std::make_unique<ResourceSystem>(*this);
    _audio = std::make_unique<AudioSystem>(*this);
    _counter = std::make_unique<FrameCounter>();
    _graphics = std::make_unique<GraphicsSystem>(*this);
    _input = std::make_unique<InputSystem>(*this);
    _imgui = std::make_unique<ImGuiSystem>(*this, _graphics->GetWindow(), _graphics->GetRenderer());
    _physics = std::make_unique<PhysicsSystem>(*this);
    _editor = std::make_unique<Editor>(*this);
}

Engine::~Engine()
{
    SDL_Quit();
    Print::Info("Shutting down BlueMetal...");
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

Editor& Engine::GetEditor()
{
    return *_editor.get();
}

} // namespace bl
