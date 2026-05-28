#include "VulkanRenderPass.h"


namespace bl {

VulkanRenderPass::VulkanRenderPass(VulkanDevice* device)
{

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    createInfo.


    vkCreateRenderPass(device, &createInfo, nullptr, &_renderPass);

}

}