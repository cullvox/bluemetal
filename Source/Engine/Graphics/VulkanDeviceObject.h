#pragma once

#include "VulkanDevice.h"

namespace bl
{

class VulkanDeviceObject
{
    VulkanDevice* _device;
public:
    VulkanDeviceObject(VulkanDevice* device)
        : _device(device)
    {
    }

    virtual ~VulkanDeviceObject() = default;

    VulkanDevice* GetDevice() const 
    { 
        return _device; 
    }
};

} // namespace bl