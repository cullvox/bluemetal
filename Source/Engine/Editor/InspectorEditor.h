#pragma once

#include "Editor.h"

namespace bl {

class InspectorEditor : public Editor {
protected:
    void OnSelectedNodeChanged() override;

public:
    InspectorEditor(Engine& engine, EditorSystem& system);
    ~InspectorEditor();

    void Draw(bl::RenderData& rd) override;
};

}