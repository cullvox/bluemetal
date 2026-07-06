#pragma once

#include "Editor.h"
#include "Graphics/RenderData.h"

namespace bl {

class VulkanViewport;

class ViewportEditor : public Editor {

    VkDescriptorSet _geometryColorDescriptor;
    std::unique_ptr<Viewport> _viewport;

    std::array<VkDescriptorSet, VulkanConfig::maxFramesInFlight> _viewportDescriptorDeleter = {};

    void OnPreViewportResized(Viewport* viewport);
    void OnPostViewportResized(Viewport* viewport);

public:
   ViewportEditor(Engine& engine, EditorSystem& system); 
   ~ViewportEditor();

    virtual void Draw(RenderData& rd);

};

} // namespace bl
