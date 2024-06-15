#pragma once

#include "Device.h"
#include "Image.h"
#include "RenderPass.h"
#include "vulkan/vulkan_core.h"

namespace bl {

class GeometryPass : public RenderPass {
public:
    GeometryPass(Device* device);
    ~GeometryPass();

    virtual VkRenderPass Get() override;
    virtual void Recreate(VkExtent2D extent, uint32_t imageCount) override;
    virtual void Begin(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) override;
    virtual void NextSubpass(VkCommandBuffer cmd) override;
    virtual void End(VkCommandBuffer cmd) override;

private:

};

}