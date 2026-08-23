#include "EditorSystem.h"
#include "Engine/Engine.h"
#include "ImGui/imgui_internal.h"
#include "Scene/Node.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Renderer.h"
#include "ImGui/imgui.h"

namespace bl {

EditorSystem::EditorSystem()
    : System()
    , _selectedNode(nullptr)
{

}

EditorSystem::~EditorSystem()
{
}

EditorSystem* EditorSystem::Get()
{
    static EditorSystem system;
    return &system;
}

void EditorSystem::SetSelectedNode(Node* node)
{
    _selectedNode = node;
    //_debug.OnSelectedNodeChanged();
    //_settings.OnSelectedNodeChanged();
    //_hierarchy.OnSelectedNodeChanged();
    //_inspector.OnSelectedNodeChanged();
}

Node* EditorSystem::GetSelectedNode() const
{
    return _selectedNode;
}

void EditorSystem::Draw(RenderData& rd)
{
    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File")) {

        if (ImGui::MenuItem("Exit")) {
            // GetEngine().GetGraphics().GetWindow()->RequestClose();
        }

        if (ImGui::MenuItem("Save Scene")) {
        }

        if (ImGui::MenuItem("Load Scene")) {
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        static bool showDebug = false;
        ImGui::MenuItem("Debug Window", nullptr, &showDebug);
        _debug.Show(showDebug);


        static bool showSettings = false;
        ImGui::MenuItem("Settings Window", nullptr, &showSettings);
        _settings.Show(showSettings);
    
        static bool showHierarchy = true;
        ImGui::MenuItem("Hierarchy Window", nullptr, &showHierarchy);
        _hierarchy.Show(showHierarchy);

        static bool showInspector = true;
        ImGui::MenuItem("Inspector Window", nullptr, &showInspector);
        _inspector.Show(showInspector);

        static bool showToolbar = true;
        ImGui::MenuItem("Toolbar", nullptr, &showToolbar);
        _toolbar.Show(showToolbar);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    _debug.Draw(rd);
    _settings.Draw(rd);
    _hierarchy.Draw(rd);
    _inspector.Draw(rd);
    _toolbar.Draw(rd);

    for (auto& vp : _viewports)
        vp.Draw(rd);
}


ViewportEditor* EditorSystem::AddViewport()
{
    auto& viewport = _viewports.emplace_back();

    for (int i = 0; i < _viewports.size(); i++)
        _viewports[i].SetIndex(i);

    return &viewport;
}

//std::span<ViewportEditor> EditorSystem::GetViewports()
//{
//    return _viewports;
//}

void EditorSystem::RemoveViewport(ViewportEditor* viewport)
{
    // auto it = std::find_if(_viewports.begin(), _viewports.end(), [viewport](auto& vp){return vp.get() == viewport;});
    auto it = std::find_if(_viewports.begin(), _viewports.end(), [viewport](auto& vp){ return &vp == viewport; });
    if (it == _viewports.end())
    {
        Print::Error("Cannot remove an invalid viewport.");
        return;
    }

    _viewports.erase(it);
}

} // namespace bl
