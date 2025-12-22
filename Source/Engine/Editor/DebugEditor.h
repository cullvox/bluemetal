#pragma once

#include <cstdint>

namespace bl {

class Editor;
class FrameCounter;
class RenderData;

class DebugEditor {
    Editor& _editor;
    bool _open;
    FrameCounter& _frameCounter;
    uint32_t _vulkanInstanceVersion;
public:
    DebugEditor(Editor& editor);
    ~DebugEditor();

    void Show(bool visible);
    void Draw(RenderData& rd);
};

} // namespace bl