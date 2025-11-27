#include "Engine/Engine.h"
#include "Core/Print.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanWindow.h"
#include "Math/Rect.h"
#include "argparse/argparse.hpp"

namespace bl {

Engine::Engine(int argc, const char** argv)
    : _sdl()
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

}

Engine::~Engine()
{
    Print::Info("Shutting down BlueMetal...");
}

void Engine::Initialize()
{
    Print::Info("Initializing BlueMetal v{}", bl::to_string(bl::engineVersion));

    _resourceManager = std::make_unique<ResourceSystem>(*this);
    _audio = std::make_unique<AudioSystem>(*this);
    _graphics = std::make_unique<GraphicsSystem>(*this);
    _input = std::make_unique<InputSystem>(*this);
    _imgui = std::make_unique<ImGuiSystem>(*this, _graphics->GetWindow(), _graphics->GetRenderer());
}

void Engine::Shutdown()
{
    Print::Info("Shutting down BlueMetal...");
    _imgui.reset();
    _graphics.reset();
    _audio.reset();
    _resourceManager.reset();
}

FrameCounter& Engine::GetFrameCounter()
{
    return _counter;
}

ResourceSystem* Engine::GetResourceManager()
{
    return _resourceManager.get();
}

GraphicsSystem* Engine::GetGraphics()
{
    return _graphics.get();
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

} // namespace bl
