#include "argparse/argparse.hpp"

#include "Audio/AudioSystem.h"
#include "Core/ClassDB.h"
#include "Core/FrameCounter.h"
#include "Core/Object.h"
#include "Core/Print.h"
#include "Core/Profiler.h"
#include "Editor/EditorSystem.h"
#include "Engine/Engine.h"
#include "Engine/SDL.h"
#include "EngineVars.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanWindow.h"
#include "ImGui/ImGuiSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Resources/Material.h"
#include "Resources/ResourceSystem.h"
#include "Scene/AudioListener3D.h"
#include "Scene/AudioSource3D.h"
#include "Scene/Camera3D.h"
#include "Scene/CharacterBody3D.h"
#include "Scene/FlyCamera3D.h"
#include "Scene/MeshInstance3D.h"
#include "Scene/MultiMeshInstance3D.h"
#include "Scene/Node.h"
#include "Scene/Node3D.h"
#include "Scene/NodeFilter.h"
#include "Scene/NodeFilterFrustumCull.h"
#include "Scene/NodeFilterIterator.h"
#include "Scene/Orbit3D.h"
#include "Scene/PhysicsBody3D.h"
#include "Scene/RigidBody3D.h"
#include "Scene/SceneExporter.h"
#include "Scene/SceneSystem.h"
#include "Scene/SceneSystem.h"
#include "Scene/Sky3D.h"
#include "Scene/SkinnedMeshInstance3D.h"
#include "Social/Discord.h"
#include "Window/Input.h"

#include "Jolt/Core/Core.h"

namespace bl {

Engine::Engine(int argc, const char** argv)
{

    // Register all engine classes with the class DB.
    _classDB = std::make_unique<ClassDB>(*this);
    RegisterClasses();

    // Print out a pretty little initialization message.
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
    Print::Raw(fmt::fg(fmt::color::orange), "Build Date: {}\n", __DATE__ " " __TIME__);
    Print::NewLine();
    Print::Raw("Using:\n");
    Print::Raw("    SDL " BL_STRINGIFY(SDL_MAJOR_VERSION) "." BL_STRINGIFY(SDL_MINOR_VERSION) "\n");
    Print::Raw("    Vulkan Header " BL_STRINGIFY(VK_HEADER_VERSION) "\n");
    Print::Raw("    Jolt Physics " BL_STRINGIFY(JPH_VERSION_MAJOR) "." BL_STRINGIFY(JPH_VERSION_MINOR) "." BL_STRINGIFY(JPH_VERSION_PATCH) "\n");
    Print::Raw("    FMOD {}.{:0>2}.{:0>2}\n", (FMOD_VERSION >> 16) & 0xFFFF, (FMOD_VERSION >> 8) & 0xFF, FMOD_VERSION & 0xFF);
    Print::NewLine();
    Print::NewLine();


    // Parse out the programs arguments for any relevant to the engine.
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

    // Initialize SDL, it's used all over the place.
    uint32_t flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS;

    if (SDL_Init(flags) == false) {
        throw std::runtime_error("Could not initialize SDL!");
    }

    // Initialize all the in engine systems.
    // Order of initialization here matters, some systems are dependant on others existing.

    _vars = std::make_unique<EngineVars>();
    _resourceManager = std::make_unique<ResourceSystem>(*this);
    _audio = std::make_unique<AudioSystem>(*this);
    _counter = std::make_unique<FrameCounter>();
    _graphics = std::make_unique<GraphicsSystem>(*this);
    _input = std::make_unique<InputSystem>(*this);
    _imgui = std::make_unique<ImGuiSystem>(*this, _graphics->GetWindow(), _graphics->GetRenderer());
    _physics = std::make_unique<PhysicsSystem>(*this);
    _scenes = std::make_unique<SceneSystem>(*this);
    _editorSystem = std::make_unique<EditorSystem>(*this);
    _discord = std::make_unique<DiscordSystem>(*this);

    // Setup the debug renderer materials.
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

EditorSystem& Engine::GetEditorSystem()
{
    return *_editorSystem.get();
}

DiscordSystem& Engine::GetDiscord()
{
    return *_discord.get();
}

ClassDB& Engine::GetClassDB()
{
    return *_classDB.get();
}

Profiler& Engine::GetProfiler()
{
    static Profiler profiler;
    return profiler;
}

void Engine::RegisterClasses()
{

    // Resources
    _classDB->Register<Resource>();
    _classDB->Register<MaterialInstance>();
    _classDB->Register<Material>();

    // Nodes/Scene
    _classDB->Register<AudioListener3D>();
    _classDB->Register<AudioSource3D>();
    _classDB->Register<Camera3D>();
    _classDB->Register<CharacterBody3D>();
    _classDB->Register<FlyCamera3D>();
    _classDB->Register<MeshInstance3D>();
    _classDB->Register<MultiMeshInstance3D>();
    _classDB->Register<Node>();
    _classDB->Register<Node3D>();
    // _classDB->Register<NodeFilter>();
    // _classDB->Register<NodeFilterFrustumCull>();
    // _classDB->Register<NodeFilterIterator>();
    _classDB->Register<Orbit3D>();
    _classDB->Register<PhysicsBody3D>();
    _classDB->Register<Sky3D>();
    //_classDB->Register<RigidBody3D>();
    //_classDB->Register<SceneExporter>();
    //_classDB->Register<SkinnedMeshInstance3D>();

}

} // namespace bl
