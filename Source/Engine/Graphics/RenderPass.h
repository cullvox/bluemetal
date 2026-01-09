#pragma once


namespace bl {

class RenderPass {
public:
    RenderPass(VulkanSwapchain&);
    virtual ~RenderPass() = default;

    virtual void Record(RenderData& renderData) = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;

};

}