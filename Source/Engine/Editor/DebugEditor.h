#pragma once

#include "Editor.h"

namespace bl {

class Editor;
class FrameCounter;
class RenderData;

class DebugEditor : public Editor {
    bool _open;
    uint32_t _vulkanInstanceVersion;
public:
    DebugEditor();
    ~DebugEditor();

    void Show(bool visible);
    void Draw(RenderData& rd);
};

} // namespace bl