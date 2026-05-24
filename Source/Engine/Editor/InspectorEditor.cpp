#include "InspectorEditor.h"
#include "Core/Variant.h"
#include "Scene/Node.h"
#include "ImGui/imgui.h"
#include "Core/ClassDB.h"
#include "EditorSystem.h"
#include "Engine/Engine.h"
#include "ImGui/ImGuizmo.h"
#include "Graphics/RenderData.h"
#include "Scene/Node3D.h"

namespace bl {

InspectorEditor::InspectorEditor(Engine& engine, EditorSystem& system)
    : Editor(engine, system)
{
}

InspectorEditor::~InspectorEditor()
{
}

void InspectorEditor::OnSelectedNodeChanged()
{
    // Nothing to do here for now.
}

void InspectorEditor::DrawProperties(Object* object, std::span<Property*> properties)
{
    for (const auto& prop : properties) {
        // Skip non-editor properties.
        if (!prop->HasFlag(PropertyFlags::Editor)) {
            continue;
        }

        ImGui::Text("%s", prop->GetName().data());

        ImGui::SameLine(); // Align the editor to the right side.

        // Align to the right, 150 px from the right edge of the window.
        float available = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 200.0f);
        ImGui::SetNextItemWidth(-1.0f);

        auto var = prop->Get(object);
        std::visit([&](auto&& value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, EnumValue>) {
                auto current = GetEngine().GetClassDB().GetEnumValueName(value.enumName, value.value);
                auto enumValues = GetEngine().GetClassDB().GetEnumValues(value.enumName);

                if (ImGui::BeginCombo(("##" + std::string(prop->GetName())).c_str(), current.data())) {
                    for (int i = 0; i < enumValues.size(); i++) {

                        const bool isSelected = (value.value == i);
                        if (ImGui::Selectable(enumValues[i].first.data(), isSelected)) {
                            current = enumValues[i].first.data();
                            prop->Set(object, EnumValue{value.enumName, enumValues[i].second});
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            } else if constexpr (std::is_same_v<T, float>) {
                ImGui::DragFloat(("##" + std::string(prop->GetName())).c_str(), &value, 0.01f);
                prop->Set(object, value);
            } else if constexpr (std::is_same_v<T, int>) {
                ImGui::DragInt(("##" + std::string(prop->GetName())).c_str(), &value);
                prop->Set(object, value);
            } else if constexpr (std::is_same_v<T, bool>) {
                ImGui::Checkbox(("##" + std::string(prop->GetName())).c_str(), &value);
                prop->Set(object, value);
            } else if constexpr (std::is_same_v<T, std::string>) {
                char buffer[256];
                strncpy(buffer, value.c_str(), sizeof(buffer));
                if (ImGui::InputText(("##" + std::string(prop->GetName())).c_str(), buffer, sizeof(buffer))) {
                    prop->Set(object, std::string(buffer));
                }
            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                ImGui::DragFloat3(("##" + std::string(prop->GetName())).c_str(), &value.x, 0.01f);
                prop->Set(object, value);
            } else if constexpr (std::is_same_v<T, glm::quat>) {
                glm::vec3 euler = glm::degrees(glm::eulerAngles(value));
                ImGui::DragFloat3(("##" + std::string(prop->GetName())).c_str(), &euler.x, 0.01f);
                value = glm::quat(glm::radians(euler));
                prop->Set(object, value);
            } else if constexpr (std::is_same_v<T, glm::vec2>) {
                ImGui::DragFloat2(("##" + std::string(prop->GetName())).c_str(), &value.x, 0.01f);
                prop->Set(object, value);
            } else if constexpr (std::is_same_v<T, glm::vec4>) {

                bool changed = false;
                if (prop->HasFlag(PropertyFlags::Color)) {

                    std::string popupId = ("##Picker" + std::string(prop->GetName()));
                    if (ImGui::ColorButton(("##" + std::string(prop->GetName())).c_str(), *reinterpret_cast<ImVec4*>(&value))) {
                        ImGui::OpenPopup(popupId.c_str());
                    }

                    if (ImGui::BeginPopup(popupId.c_str())) {
                        if (ImGui::ColorPicker4(("##ColorPicker" + std::string(prop->GetName())).c_str(), glm::value_ptr(value))) { 
                            changed = true;
                        }

                        if (ImGui::Button("##Close")) ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                    }
                } else {
                    changed = ImGui::DragFloat4(("##" + std::string(prop->GetName())).c_str(), &value.x, 0.01f);
                }

                prop->Set(object, value);
            } else if constexpr (std::is_same_v<T, Object*>) {

                // Resource objects can be edited.
                if (value && value->IsA("Resource")) {
                    DrawObjectProperties(value);
                }

            } else {
                ImGui::Text("Unsupported property type.");
            }
        }, var);
    }
}

void InspectorEditor::DrawObjectProperties(Object* object)
{

    // Draw any instance properties.
    ImGui::TextDisabled("%s", "Instance Properties");
    ImGui::Separator();

    DrawProperties(object, object->GetInstanceProperties());

    // Iterate throught all the properties of the selected node and it's parent classes, and display them in the inspector too.
    std::string_view className = object->GetClassName();
    while (!className.empty()) {
        if (className == "Object") {
            // Stop at Object, since it's the base class for all objects and we don't want to show its properties.
            break;
        }

        ImGui::TextDisabled("%s", className.data());
        ImGui::Separator();




        auto properties =  GetEngine().GetClassDB().GetClassProperties(className);

        DrawProperties(object, properties);

        className = GetEngine().GetClassDB().GetClassParent(className);
    }
}

void InspectorEditor::Draw(RenderData& rd)
{
    ImGui::Begin("Inspector");

    auto selectedNode = GetSelectedNode();

    if (!selectedNode) {
        ImGui::Text("No node selected.");
        ImGui::End();
        return;
    }

    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    DrawObjectProperties(selectedNode);

    if (selectedNode->IsA("Node3D")) {
        auto node = static_cast<Node3D*>(selectedNode);
        auto localMatrix = node->GetWorldMatrix();
        auto projection = rd.GetProjectionMatrix();
        projection[1][1] *= -1.0f; // Flip back from vulkans default.
        if (ImGuizmo::Manipulate(glm::value_ptr(rd.GetViewMatrix()), glm::value_ptr(projection), ImGuizmo::ROTATE, ImGuizmo::WORLD, glm::value_ptr(localMatrix))) {

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

}