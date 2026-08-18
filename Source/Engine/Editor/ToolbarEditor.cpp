#include "ToolbarEditor.h"
#include "Editor/Editor.h"
#include "Scene/Node3D.h"

#include "ImGui/imgui.h"
#include "ImGui/ImGuizmo.h"

namespace bl {

ToolbarEditor::ToolbarEditor()
    : Editor()
{
}

ToolbarEditor::~ToolbarEditor()
{
}

void ToolbarEditor::SetTransformMode(TransformMode mode)
{
    _transformMode = mode;
}

void ToolbarEditor::Draw(RenderData& rd)
{

    bool isShown = GetShown();
    ImGui::Begin("Toolbar", &isShown, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);


    if (ImGui::Button("Translate")) {
        _transformMode = TransformMode::eTranslate;
    }

    ImGui::SameLine();

    if (ImGui::Button("Rotatate")) {
        _transformMode = TransformMode::eRotate;
    }

    ImGui::SameLine();

    if (ImGui::Button("Scale")) {
        _transformMode = TransformMode::eScale;
    }

    auto selectedNode = GetSelectedNode();
    if (selectedNode && selectedNode->IsA("Node3D")) {
        auto node = static_cast<Node3D*>(selectedNode);
        auto localMatrix = node->GetWorldMatrix();
        auto projection = rd.GetProjectionMatrix();
        projection[1][1] *= -1.0f; // Flip back from vulkans default.

        ImGuizmo::OPERATION mode = ImGuizmo::TRANSLATE;

        switch (_transformMode) {
        case TransformMode::eTranslate: mode = ImGuizmo::TRANSLATE; break;
        case TransformMode::eRotate: mode = ImGuizmo::ROTATE; break;
        case TransformMode::eScale: mode = ImGuizmo::SCALE; break;
        }

        ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

        if (ImGuizmo::Manipulate(glm::value_ptr(rd.GetViewMatrix()), glm::value_ptr(projection), mode, ImGuizmo::WORLD, glm::value_ptr(localMatrix))) {

            auto parent = dynamic_cast<Node3D*>(node->GetParent());
            auto parentMatrix = parent ? parent->GetWorldMatrix() : glm::identity<glm::mat4>();
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 position;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(localMatrix, scale, rotation, position, skew, perspective);
            node->SetWorldScale(scale);
            node->SetWorldRotation(rotation);
            node->SetWorldPosition(position);
        }
    }

    ImGui::End();
}

} // namespace bl