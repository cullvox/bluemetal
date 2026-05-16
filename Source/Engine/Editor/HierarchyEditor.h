#pragma once

#include "Editor.h"

namespace bl {

class HierarchyEditor : public Editor {
    Node* _rootNode;

protected:
    void OnSelectedNodeChanged() override;

public:
    HierarchyEditor(Engine& engine, EditorSystem& system);
    ~HierarchyEditor();

    virtual void Draw(RenderData& rd) override;
    void SetRootNode(Node* node);

};

} // namespace bl
