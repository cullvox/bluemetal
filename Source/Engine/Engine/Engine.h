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
class EditorSystem;
class ClassDB;
class Profiler;

class Engine {
    std::unique_ptr<FrameCounter> _counter;
    std::unique_ptr<Window> _window;
    std::unique_ptr<EngineVars> _vars;

    void RegisterClasses();

    Engine();
    ~Engine();

public:
    static Engine* Get();

    /** @brief Sets the command line arguments in the engine, sets engine variables.   */
    void SetArguments(int argc, const char** argv);

    /** @brief Initializes the engine once, prepare all configuration before this. */
    void Initialize();

    /** @brief Halts engine processing and destroys all engine systems. */
    void Shutdown();

    EngineVars* GetVars();
    FrameCounter* GetFrameCounter();
    ResourceSystem* GetResourceSystem();
    GraphicsSystem* GetGraphics();
    InputSystem* GetInput();
    ImGuiSystem* GetImGui();
    AudioSystem* GetAudio();
    Window* GetWindow();
    Renderer* GetRenderer();
    PhysicsSystem* GetPhysics();
    SceneSystem* GetSceneSystem();
    EditorSystem* GetEditorSystem();
    DiscordSystem* GetDiscord();
    ClassDB* GetClassDB();
    Profiler* GetProfiler();
};

} // namespace bl
