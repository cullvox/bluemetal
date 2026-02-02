#pragma once

#include <memory>

namespace bl {

class EngineVars;
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
class SceneSystem;
class DiscordSystem;

class Engine {
    std::unique_ptr<FrameCounter> _counter;
    std::unique_ptr<ResourceSystem> _resourceManager;
    std::unique_ptr<GraphicsSystem> _graphics;
    std::unique_ptr<ImGuiSystem> _imgui;
    std::unique_ptr<AudioSystem> _audio;
    std::unique_ptr<InputSystem> _input;
    std::unique_ptr<Window> _window;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<PhysicsSystem> _physics;
    std::unique_ptr<SceneSystem> _scenes;
    std::unique_ptr<Editor> _editor;
    std::unique_ptr<DiscordSystem> _discord;
    std::unique_ptr<EngineVars> _vars;

public:
    Engine(int argc, const char** argv);
    ~Engine();

    EngineVars& GetVars();
    FrameCounter& GetFrameCounter();
    ResourceSystem* GetResourceSystem();
    GraphicsSystem& GetGraphics();
    InputSystem* GetInput();
    ImGuiSystem* GetImGui();
    AudioSystem* GetAudio();
    Window* GetWindow();
    Renderer* GetRenderer();
    PhysicsSystem& GetPhysics();
    SceneSystem* GetSceneSystem();
    Editor& GetEditor();
    DiscordSystem& GetDiscord();

    void Log()
    {
        
    }
    void LogError();
    void LogDebug();
};

} // namespace bl
