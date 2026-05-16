#pragma once

#include "Core/Object.h"

namespace bl {

class Engine;
class Node;
class RenderData;
class EditorSystem;

class Editor : public Object {
    Engine& _engine;
    EditorSystem& _system;
    bool _shown;

protected:
    virtual void OnSelectedNodeChanged();
    Engine& GetEngine() const;
    EditorSystem& GetSystem() const;
    bool GetShown() const;

public:
    Editor(Engine& engine, EditorSystem& system);
    ~Editor();

    virtual void Draw(RenderData& rd);
    void Show(bool visible) { _shown = visible; }

    Node* GetSelectedNode() const;
    void SetSelectedNode(Node* node);

};

}