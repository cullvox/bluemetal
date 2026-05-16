#pragma once

#include "Editor.h"

#include "ImGui/imgui.h"

namespace bl {

class LogEditor : public Editor {
    ImGuiTextBuffer _logBuffer;

public:
    LogEditor(Engine& engine, EditorSystem& system);
    ~LogEditor();

    void Draw(bl::RenderData& rd) override;
};

} // namespace bl