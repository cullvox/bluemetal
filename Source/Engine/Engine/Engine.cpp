#include "Resources/NoiseTexture2D.h"
#include "argparse/argparse.hpp"

#include "Audio/AudioSystem.h"
#include "Core/ClassDB.h"
#include "Core/FrameCounter.h"
#include "Core/Object.h"
#include "Core/Print.h"
#include "Core/Profiler.h"
#include "Core/Version.h"
#include "Editor/EditorSystem.h"
#include "Engine/Engine.h"
#include "Engine/SDL.h"
#include "EngineVars.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "ImGui/ImGuiSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Resources/Material.h"
#include "Resources/Shader.h"
#include "Resources/Model.h"
#include "Resources/Texture2D.h"
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
#include "Scene/SkinnedMeshInstance3D.h"
#include "Scene/Sky3D.h"
#include "Social/Discord.h"
#include "Window/Input.h"

#include "Jolt/Core/Core.h"

namespace bl {

Engine::Engine()
{
    // Register all engine classes with the class DB.
    RegisterClasses();

    _vars = std::make_unique<EngineVars>();
}

Engine::~Engine()
{
    SDL_Quit();
    Print::Info("Shutting down BlueMetal...");
}

Engine* Engine::Get()
{
    static Engine engine;
    return &engine;
}

void Engine::SetArguments(int argc, const char** argv)
{
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
}

void Engine::Initialize()
{
    // Initialize SDL, it's used all over the place.
    uint32_t flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS;

    if (SDL_Init(flags) == false) {
        throw std::runtime_error("Could not initialize SDL!");
    }

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


    //bool verbose = program->get<bool>("--verbose");
    //Print::EnableVerboseLogging(verbose);

    // Initialize all the in engine systems.
    // Order of initialization here matters, some systems are dependant on others existing.

    _counter = std::make_unique<FrameCounter>();
    _window = std::make_unique<Window>("Maginvox", Rect2D{{0, 0}, {1920, 1080}}, false);

    // Setup the debug renderer materials.
    auto rs = ResourceSystem::Get();
    auto gs = GraphicsSystem::Get();
    auto _pointMaterial = rs->Load<Material>("Resources/Materials/DebugPoint.json");
    auto _lineMaterial = rs->Load<Material>("Resources/Materials/DebugLine.json");
    auto _triangleMaterial = rs->Load<Material>("Resources/Materials/DebugTriangle.json");
    gs->GetRenderer()->SetDebugMaterialInstance(_pointMaterial.lock()->GetVulkanMaterial(), _lineMaterial.lock()->GetVulkanMaterial(), _triangleMaterial.lock()->GetVulkanMaterial());
}

EngineVars* Engine::GetVars()
{
    return _vars.get();
}

FrameCounter* Engine::GetFrameCounter()
{
    return _counter.get();
}


Window* Engine::GetWindow()
{
    return _window.get();
}

Profiler* Engine::GetProfiler()
{
    static Profiler profiler;
    return &profiler;
}

void Engine::RegisterClasses()
{

    ClassDB* db = ClassDB::Get();

    // Resources
    db->Register<Object>();

    db->Register<Resource>();
    db->Register<MaterialInstance>();
    db->Register<Material>();
    db->Register<Sampler>();
    db->Register<Shader>();
    db->Register<Sound>();
    db->Register<Texture>();
    db->Register<Texture2D>();
    db->Register<NoiseTexture2D>();
    db->Register<Model>();

    // Nodes/Scene
    db->Register<Node>();
    db->Register<Node3D>();
    db->Register<AudioListener3D>();
    db->Register<AudioSource3D>();
    db->Register<Camera3D>();
    db->Register<PhysicsBody3D>();
    db->Register<CharacterBody3D>();
    db->Register<FlyCamera3D>();
    db->Register<MeshInstance3D>();
    db->Register<MultiMeshInstance3D>();
    // _classDB->Register<NodeFilter>();
    // _classDB->Register<NodeFilterFrustumCull>();
    // _classDB->Register<NodeFilterIterator>();
    db->Register<Orbit3D>();
    db->Register<Sky3D>();
    //_classDB->Register<RigidBody3D>();
    //_classDB->Register<SceneExporter>();
    //_classDB->Register<SkinnedMeshInstance3D>();

}

} // namespace bl
