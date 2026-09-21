#pragma once

#include "Editor.h"
#include "Core/Reference.h"
#include "Graphics/Viewport.h"
#include "Graphics/RenderData.h"
#include "Graphics/VulkanConfig.h"
#include <vulkan/vulkan_core.h>

namespace bl {

class VulkanViewport;

class ViewportEditor : public Editor {

    VkDescriptorSet _geometryColorDescriptor;
    Ref<Viewport> _viewport;
    std::array<VkDescriptorSet, VulkanConfig::maxFramesInFlight> _viewportDescriptorDeleter = {};
    int32_t _id = 0;

    void OnPreViewportResized(Viewport& viewport);
    void OnPostViewportResized(Viewport& viewport);

public:
    ViewportEditor();
    ViewportEditor(const ViewportEditor& copy);
    ViewportEditor(ViewportEditor&& move);
    ~ViewportEditor();

    ViewportEditor& operator=(const ViewportEditor& other);
    ViewportEditor& operator=(ViewportEditor&& other);

    void SetIndex(int32_t id) { _id = id; };
    void SetViewport(Ref<Viewport> viewport);
    Ref<Viewport> GetViewport();

    virtual void Draw(RenderData& rd);

};

} // namespace bl
