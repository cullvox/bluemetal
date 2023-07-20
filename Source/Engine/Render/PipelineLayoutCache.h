#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

class blPipelineLayoutCache
{
public:
    explicit blPipelineLayoutCache(std::shared_ptr<blDevice> device);
    ~blPipelineLayoutCache();

    VkPipelineLayout createLayout(const VkPipelineLayoutCreateInfo& createInfo);

private:

    std::map<

}