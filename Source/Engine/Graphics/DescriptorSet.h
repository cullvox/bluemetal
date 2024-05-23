#pragma once

#include "Vulkan.h"

namespace bl
{

class Device;
class DescriptorSet
{
public:
    DescriptorSet(Device* device);
    ~DescriptorSet();

private:
    Device* _device;
    VkDescriptorSet _set;
};

} // namespace bl