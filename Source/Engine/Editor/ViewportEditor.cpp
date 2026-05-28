#include "ViewportEditor.h"

#include "Graphics/VulkanViewport.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_vulkan.h"

#include "Engine/Engine.h"
#include "Graphics/Renderer.h"
#include "Resources/ResourceSystem.h"
#include "Resources/Sampler.h"

namespace bl {


ViewportEditor::ViewportEditor(Engine& engine, EditorSystem& system)
    : Editor(engine, system)
{

    // Ask the renderer to allocate a couple renderable images.

    auto renderer = engine.GetRenderer();
    auto defaultSampler = engine.GetResourceSystem()->Load<Sampler>("Resources/Samplers/Default.json");

    auto geometryColor = renderer->GetColorImageView();
    auto geometryColorDescriptor = ImGui_ImplVulkan_AddTexture(defaultSampler.lock()->Get(), geometryColor->Get(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    _geometryColorDescriptor = geometryColorDescriptor;
}

ViewportEditor::~ViewportEditor()
{
    auto renderer = GetEngine().GetRenderer();
    auto geometryColor = renderer->GetColorImageView();

    ImGui_ImplVulkan_RemoveTexture(_geometryColorDescriptor);
}

void ViewportEditor::Draw(RenderData& rd)
{

    bool isOpen = GetShown();
    ImGui::Begin("Viewport", &isOpen);

    ImVec2 region = ImGui::GetContentRegionAvail();

    ImGui::Image(_geometryColorDescriptor, region);
        
    ImGui::End();

}

}