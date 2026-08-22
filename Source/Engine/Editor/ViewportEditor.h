#pragma once

#include "Editor.h"
#include "Graphics/RenderData.h"

namespace bl {

class VulkanViewport;

class ViewportEditor : public Editor {

    VkDescriptorSet _geometryColorDescriptor;
    std::unique_ptr<Viewport> _viewport;

    std::array<VkDescriptorSet, VulkanConfig::maxFramesInFlight> _viewportDescriptorDeleter = {};

    int32_t _id = 0;

    void OnPreViewportResized(Viewport* viewport);
    void OnPostViewportResized(Viewport* viewport);

public:
    ViewportEditor();
    ViewportEditor(ViewportEditor&& move) = default;
    ~ViewportEditor();

    void SetIndex(int32_t id) { _id = id; };

    ViewportEditor& operator=(ViewportEditor&& move) = default;

    virtual void Draw(RenderData& rd);

};

} // namespace bl
