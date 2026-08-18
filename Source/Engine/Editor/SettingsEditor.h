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
    std::vector<VkPresentModeKHR> _presentModes;
    std::vector<VkSampleCountFlagBits> _multisampleModes;
    bool _show;
    bool _collapsed;
    bool _enableEditor;

public:
    SettingsEditor();
    ~SettingsEditor();

    void SetShow(bool show);
    void SetCollapsed(bool collapsed);
    void Draw(RenderData& renderData);

};

} // namespace bl