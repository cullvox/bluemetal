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
#include "Window/Window.h"
#include <vulkan/vulkan_core.h>

namespace bl {


ViewportEditor::ViewportEditor()
    : Editor()
{

    // Ask the renderer to allocate a couple renderable images.

    auto renderer = GraphicsSystem::Get()->GetRenderer();
    auto defaultSampler = ResourceSystem::Get()->Load<Sampler>("Resources/Samplers/Default.json");

    _viewport = std::make_unique<Viewport>(GraphicsSystem::Get()->GetRenderer(), VkExtent2D{1, 1});
    auto geometryColor = _viewport->GetRenderedImageView();
    auto geometryColorDescriptor = ImGui_ImplVulkan_AddTexture(defaultSampler.lock()->Get(), geometryColor, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    _viewport->onPreViewportResized.AddRaw(this, &ViewportEditor::OnPreViewportResized);
    _viewport->onPostViewportResized.AddRaw(this, &ViewportEditor::OnPostViewportResized);

    _viewport->SetRenderingPriority(100);

    renderer->AddViewport(_viewport.get());

    _geometryColorDescriptor = geometryColorDescriptor;
}

ViewportEditor::~ViewportEditor()
{
    auto renderer = GraphicsSystem::Get()->GetRenderer();
    auto geometryColor = _viewport->GetRenderedImageView();

    ImGui_ImplVulkan_RemoveTexture(_geometryColorDescriptor);
}

void ViewportEditor::OnPreViewportResized(Viewport* viewport)
{
    // Add this descriptor to the deleter queue for this frame.

    //ImGui_ImplVulkan_RemoveTexture(_geometryColorDescriptor);
    _viewportDescriptorDeleter[GraphicsSystem::Get()->GetRenderer()->GetRenderData().GetCurrentFrame()] = _geometryColorDescriptor;
}

void ViewportEditor::OnPostViewportResized(Viewport* viewport)
{
    auto newView = _viewport->GetRenderedImageView();

    // Add this descriptor to the deleter queue for this frame.
    _viewportDescriptorDeleter[GraphicsSystem::Get()->GetRenderer()->GetRenderData().GetCurrentFrame()] = _geometryColorDescriptor;

    auto defaultSampler = ResourceSystem::Get()->Load<Sampler>("Resources/Samplers/Default.json");
    _geometryColorDescriptor = ImGui_ImplVulkan_AddTexture(defaultSampler.lock()->Get(), newView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void ViewportEditor::Draw(RenderData& rd)
{
    // Clear the deleter for this frame.
    if (_viewportDescriptorDeleter[rd.GetCurrentFrame()] != VK_NULL_HANDLE)
    {
        ImGui_ImplVulkan_RemoveTexture(_viewportDescriptorDeleter[rd.GetCurrentFrame()]);
        _viewportDescriptorDeleter[rd.GetCurrentFrame()] = VK_NULL_HANDLE;
    }

    VkExtent2D extent = _viewport->GetExtent();

    glm::mat4 view = glm::identity<glm::mat4>();
    view = glm::translate(view, {0, 2.5, -10});

    glm::mat4 projection = glm::perspective(glm::radians(75.0f), static_cast<float>(extent.width) / static_cast<float>(extent.height), 0.01f, 1000.0f);
    projection[1][1] *= -1.0f;

    _viewport->SetView(view);
    _viewport->SetProjection(projection);

    bool isOpen = true;
    
    std::string name = "Viewport##" + _id;
    ImGui::Begin(name.c_str(), &isOpen);

    ImVec2 region = ImGui::GetContentRegionAvail();
    region = { std::max(1.0f, region.x), std::max(1.0f, region.y) };

    float scale = ImGui::GetWindowDpiScale(); 

    float density = SDL_GetWindowPixelDensity(GetEngine()->GetWindow()->Get());

    if (region.x * density != extent.width || region.y * density != extent.height)
    {
        _viewport->SetSize({static_cast<uint32_t>(region.x * density), static_cast<uint32_t>(region.y * density)});
    }

    ImGui::Image(_geometryColorDescriptor, region);
        
    ImGui::End();

}

}
