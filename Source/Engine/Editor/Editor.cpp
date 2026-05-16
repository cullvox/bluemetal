#include "Editor.h"
#include "EditorSystem.h"
#include "Engine/Engine.h"
#include "Core/ClassDB.h"
#include "Scene/Node.h"
#include "ImGui/imgui.h"

namespace bl {

Editor::Editor(Engine& engine, EditorSystem& system)
    : Object(engine)
    , _engine(engine)
    , _system(system)
{
}

Editor::~Editor()
{
}

bool Editor::GetShown() const
{
    return _shown;
}

void Editor::Draw(RenderData& rd)
{
}

void Editor::OnSelectedNodeChanged()
{
}

Engine& Editor::GetEngine() const 
{ 
    return _engine; 
}

EditorSystem& Editor::GetSystem() const 
{ 
    return _system; 
}

Node* Editor::GetSelectedNode() const
{
    return GetSystem().GetSelectedNode();
}

void Editor::SetSelectedNode(Node* node)
{
    GetSystem().SetSelectedNode(node);
}

} // namespace bl
