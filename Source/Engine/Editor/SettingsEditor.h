#pragma once

#include "Editor.h"

namespace bl
{

class Engine;
class RenderData;
class Renderer;
class AudioSystem;
class GraphicsSystem;
class PhysicsSystem;
class PhysicsRenderer;
class Profiler;
class FrameCounter;
class EditorSystem;

class SettingsEditor : public Editor
{
    Engine& _engine;
    Renderer* _renderer;
    AudioSystem* _audio;
    GraphicsSystem& _graphics;
    PhysicsSystem& _physics;
    PhysicsRenderer* _physicsRenderer;
    FrameCounter& _frameCounter;
    Profiler& _profiler;
    std::vector<VkPresentModeKHR> _presentModes;
    std::vector<VkSampleCountFlagBits> _multisampleModes;
    bool _show;
    bool _collapsed;
    bool _enableEditor;

public:
    SettingsEditor(Engine& engine, EditorSystem& system);
    ~SettingsEditor();

    void SetShow(bool show);
    void SetCollapsed(bool collapsed);
    void Draw(RenderData& renderData);

};

} // namespace bl