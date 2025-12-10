#pragma once

namespace bl {

class Editor;
class FrameCounter;
struct VulkanRenderData;

class DebugEditor {
    Editor& _editor;
    bool _open;
    FrameCounter& _frameCounter;
public:
    DebugEditor(Editor& editor);
    ~DebugEditor();

    void Show(bool visible);
    void Draw(VulkanRenderData& rd);
};

} // namespace bl