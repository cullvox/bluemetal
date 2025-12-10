#pragma once

#include <memory>

//#include "Audio/AudioSystem.h"
//#include "Core/Flags.h"
//#include "Core/FrameCounter.h"
#include "Engine/SDL.h"
//#include "Graphics/GraphicsSystem.h"
//#include "ImGui/ImGuiSystem.h"
//#include "Resources/ResourceSystem.h"
//#include "Physics/PhysicsSystem.h"
//#include "Window/Input.h"
//#include "Editor/Editor.h"

namespace bl {

class ResourceSystem;
class GraphicsSystem;
class ImGuiSystem;
class AudioSystem;
class InputSystem;
class Window;
class Renderer;
class PhysicsSystem;
class FrameCounter;
class Editor;

class Engine {
    SDLInitializer _sdl;
    std::unique_ptr<FrameCounter> _counter;
    std::unique_ptr<ResourceSystem> _resourceManager;
    std::unique_ptr<GraphicsSystem> _graphics;
    std::unique_ptr<ImGuiSystem> _imgui;
    std::unique_ptr<AudioSystem> _audio;
    std::unique_ptr<InputSystem> _input;
    std::unique_ptr<Window> _window;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<PhysicsSystem> _physics;
    std::unique_ptr<Editor> _editor;

public:
    Engine(int argc, const char** argv);
    ~Engine();

    void Initialize();
    void Shutdown();

    FrameCounter& GetFrameCounter();
    ResourceSystem* GetResourceSystem();
    GraphicsSystem& GetGraphics();
    InputSystem* GetInput();
    ImGuiSystem* GetImGui();
    AudioSystem* GetAudio();
    Window* GetWindow();
    Renderer* GetRenderer();
    PhysicsSystem& GetPhysics();
    Editor& GetEditor();
};

} // namespace bl
