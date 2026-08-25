#pragma once

#include "Core/Object.h"

namespace bl {

class Engine;
class Node;
class RenderData;
class EditorSystem;

class Editor : public Object {
    bool _shown;


protected:
    virtual void OnSelectedNodeChanged();
    bool GetShown() const;

public:
    Editor();
    Editor(const Editor&);
    Editor(Editor&&);
    ~Editor();

    Editor& operator=(const Editor&);
    Editor& operator=(Editor&&);

    virtual void Draw(RenderData& rd);
    void Show(bool visible) { _shown = visible; }

    Node* GetSelectedNode() const;
    void SetSelectedNode(Node* node);

};

}