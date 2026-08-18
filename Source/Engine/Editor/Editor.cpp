#include "Editor.h"
#include "EditorSystem.h"
#include "Engine/Engine.h"
#include "Core/ClassDB.h"
#include "Scene/Node.h"
#include "ImGui/imgui.h"

namespace bl {

Editor::Editor()
    : Object()
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

Node* Editor::GetSelectedNode() const
{
    return EditorSystem::Get()->GetSelectedNode();
}

void Editor::SetSelectedNode(Node* node)
{
    EditorSystem::Get()->SetSelectedNode(node);
}

} // namespace bl
