#include "ViewportEditor.h"

#include "Graphics/Viewport.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_vulkan.h"

#include "Engine/Engine.h"
#include "Graphics/Renderer.h"
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

    _viewport = std::make_unique<Viewport>(engine.GetGraphics().GetDevice(), VkExtent2D{1, 1});
    auto geometryColor = _viewport->GetColorResolveImageView();
    auto geometryColorDescriptor = ImGui_ImplVulkan_AddTexture(defaultSampler.lock()->Get(), geometryColor, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    _geometryColorDescriptor = geometryColorDescriptor;
}

ViewportEditor::~ViewportEditor()
{
    auto renderer = GetEngine().GetRenderer();
    auto geometryColor = _viewport->GetColorResolveImageView();

    ImGui_ImplVulkan_RemoveTexture(_geometryColorDescriptor);
}

void ViewportEditor::Draw(RenderData& rd)
{

    bool isOpen = GetShown();
    ImGui::Begin("Viewport", &isOpen);

    VkExtent2D extent = _viewport->GetExtent();
    ImVec2 region = ImGui::GetContentRegionAvail();
    if (region.x != extent.width || region.y != extent.height)
    {
        _viewport->SetSize({static_cast<uint32_t>(region.x), static_cast<uint32_t>(region.y)});
    }

    ImGui::Image(_geometryColorDescriptor, region);
        
    ImGui::End();

}

}