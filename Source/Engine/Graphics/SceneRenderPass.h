#pragma once

#include "Graphics/VulkanDevice.h"
#include "RenderPass.h"

namespace bl {

class SceneRenderPass : public RenderPass {
public:
    SceneRenderPass(VulkanDevice* device);
    ~SceneRenderPass();

    virtual void Render(Viewport& viewport, RenderData& renderData);
    virtual void RecreateImages(VkExtent2D extent);
};

} // namespace bl