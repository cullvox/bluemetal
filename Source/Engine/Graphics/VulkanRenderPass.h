#pragma once

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace bl
{


class VulkanRenderPass
{
private:
    VkRenderPass renderPass;

public:
    RenderPass(/* args */);
    ~RenderPass();
};

RenderPass::RenderPass(/* args */)
{
}

RenderPass::~RenderPass()
{
}

} // namespace bl