#pragma once

#include "Editor/ToolbarEditor.h"
#include "Engine/System.h"
#include "DebugEditor.h"
#include "SettingsEditor.h"
#include "HierarchyEditor.h"
#include "InspectorEditor.h"
#include "ToolbarEditor.h"
#include "ViewportEditor.h"

namespace bl {

class Node;
class RenderData;

class EditorSystem : public System {
    DebugEditor _debug;
    SettingsEditor _settings;
    HierarchyEditor _hierarchy;
    InspectorEditor _inspector;
    ToolbarEditor _toolbar;
    std::vector<ViewportEditor> _viewports;

    Node* _selectedNode;

    EditorSystem();
    ~EditorSystem();


public:

    static EditorSystem* Get();

    SettingsEditor& GetSettingsEditor() { return _settings; }
    DebugEditor& GetDebugEditor() { return _debug; }
    HierarchyEditor& GetHierarchyEditor() { return _hierarchy; }
    InspectorEditor& GetInspectorEditor() { return _inspector; }

    void SetSelectedNode(Node* node);
    Node* GetSelectedNode() const;
    void Draw(RenderData& rd);
    ViewportEditor* AddViewport();
    std::span<ViewportEditor> GetViewports();
    void RemoveViewport(ViewportEditor* viewport);

};

}