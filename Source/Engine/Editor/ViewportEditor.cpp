#include "ViewportEditor.h"

#include "Graphics/VulkanViewport.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_vulkan.h"
#include <vulkan/vulkan_core.h>

namespace bl {


ViewportEditor::ViewportEditor(Engine& engine, EditorSystem& system)
{

    // Ask the renderer to allocate a couple renderable images.
    VulkanViewport viewport;

    // Register the images with imgui.
    auto imageViews = viewport.GetImageViews();
    _viewportImageDescriptors.reserve(imageViews.size());

    for (auto imageView : imageViews)
        _viewportImageDescriptors.push_back(ImGui_ImplVulkan_AddTexture(imageView.Get(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));

}

void ViewportEditor::Draw(RenderData& rd)
{

    bool isOpen = GetShown();
    ImGui::Begin("Viewport", &isOpen);

    ImVec2 region = ImGui::GetContentRegionAvail();
    if (region.x != )

    ImGui::Image(_viewportImageDescriptors[rd.GetCurrentFrame()], )
        
    ImGui::End();

}

}