#pragma once

#include "Editor.h"
#include "Graphics/RenderData.h"

namespace bl {

class VulkanViewport;

class ViewportEditor : public Editor {

    std::vector<VkDescriptorSet> _viewportImageDescriptors;

public:
   ViewportEditor(Engine& engine, EditorSystem& system);
    ~ViewportEditor();

    virtual void Draw(RenderData& rd);

};

} // namespace bl
