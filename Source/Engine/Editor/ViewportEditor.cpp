#include "ViewportEditor.h"

#include "Graphics/Viewport.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_vulkan.h"

#include "Engine/Engine.h"
#include "Graphics/Renderer.h"
#include "ImGui/imgui_internal.h"
#include "Resources/ResourceSystem.h"
#include "Resources/Sampler.h"
#include "Graphics/Viewport.h"
#include "Graphics/GraphicsSystem.h"
#include <vulkan/vulkan_core.h>

namespace bl {


ViewportEditor::ViewportEditor(Engine& engine, EditorSystem& system)
    : Editor(engine, system)
{

    // Ask the renderer to allocate a couple renderable images.

    auto renderer = engine.GetRenderer();
    auto defaultSampler = engine.GetResourceSystem()->Load<Sampler>("Resources/Samplers/Default.json");

    _viewport = std::make_unique<Viewport>(engine.GetGraphics().GetRenderer(), VkExtent2D{1, 1});
    auto geometryColor = _viewport->GetRenderedImageView();
    auto geometryColorDescriptor = ImGui_ImplVulkan_AddTexture(defaultSampler.lock()->Get(), geometryColor, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    _viewport->onViewportResized.AddRaw(this, &ViewportEditor::OnViewportResized);

    renderer->AddViewport(_viewport.get());

    _geometryColorDescriptor = geometryColorDescriptor;
}

ViewportEditor::~ViewportEditor()
{
    auto renderer = GetEngine().GetRenderer();
    auto geometryColor = _viewport->GetRenderedImageView();

    ImGui_ImplVulkan_RemoveTexture(_geometryColorDescriptor);
}

void ViewportEditor::OnViewportResized(Viewport* viewport)
{
    auto newView = _viewport->GetRenderedImageView();

    if (_geometryColorDescriptor != VK_NULL_HANDLE)
    {
        ImGui_ImplVulkan_RemoveTexture(_geometryColorDescriptor);
    }

    auto defaultSampler = GetEngine().GetResourceSystem()->Load<Sampler>("Resources/Samplers/Default.json");
    _geometryColorDescriptor = ImGui_ImplVulkan_AddTexture(defaultSampler.lock()->Get(), newView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void ViewportEditor::Draw(RenderData& rd)
{
    VkExtent2D extent = _viewport->GetExtent();

    glm::mat4 view = glm::identity<glm::mat4>();
    view = glm::translate(view, {0, 2.5, -10});

    glm::mat4 projection = glm::perspective(glm::radians(75.0f), static_cast<float>(extent.width) / static_cast<float>(extent.height), 0.01f, 1000.0f);
    projection[1][1] *= -1.0f;

    _viewport->SetView(view);
    _viewport->SetProjection(projection);

    bool isOpen = GetShown();
    ImGui::Begin("Viewport", &isOpen);

    ImVec2 region = ImGui::GetContentRegionAvail();
    float scale = ImGui::GetWindowDpiScale(); 

    if (region.x != extent.width || region.y != extent.height)
    {
        _viewport->SetSize({static_cast<uint32_t>(region.x * scale), static_cast<uint32_t>(region.y * scale)});
    }

    ImGui::Image(_geometryColorDescriptor, region);
        
    ImGui::End();

}

}