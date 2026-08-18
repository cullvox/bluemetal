#include "HierarchyEditor.h"
#include "Scene/Node.h"
#include "ImGui/imgui.h"

namespace bl {

HierarchyEditor::HierarchyEditor()
    : Editor()
    , _rootNode(nullptr)
{
}

HierarchyEditor::~HierarchyEditor()
{
}

void HierarchyEditor::OnSelectedNodeChanged()
{
    // Nothing to do here for now.
}

void HierarchyEditor::SetRootNode(Node* node)
{
    _rootNode = node;
}

void HierarchyEditor::Draw(RenderData& rd)
{
    if (!GetShown()) return;

    ImGui::Begin("Heirarchy");

    std::function<void(bl::Node*)> doChildren;

    doChildren = [&](bl::Node* node){
        if (!node) return;

        auto& children = node->GetVecChildren();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (node == GetSelectedNode())
            flags |= ImGuiTreeNodeFlags_Selected;

        if (children.empty())
            flags |= ImGuiTreeNodeFlags_Leaf;


        bool opened = ImGui::TreeNodeEx(node->GetName().c_str(), flags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            SetSelectedNode(node);
        }

        if (opened)
        {
            ImGui::SameLine();
            ImGui::TextColored({0.45f, 0.34f, 0.45f, 1.0f}, "%s", node->GetClassName().data());


            for (const auto& child : children)
            {
                doChildren(child.get());
            }
            ImGui::TreePop();
        } else {
            ImGui::SameLine();
            ImGui::TextColored({0.45f, 0.34f, 0.45f, 1.0f}, "%s", node->GetClassName().data());
        }


    };


    doChildren(_rootNode);

    //ImGui::Text("Objects in Scene: %d", rootNode->GetChildCount());
    ImGui::End();
}

}