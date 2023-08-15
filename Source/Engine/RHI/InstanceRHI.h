#pragma once

#include "PhysicalDeviceRHI.h"
#include "DeviceRHI.h"

namespace bl {
namespace rhi {

class Instance
{
public:
    Instance();
    ~Instance();

public:


    std::vector<PhysicalDeviceRHI> GetPhysicalDevices();
    DeviceRHI CreateDevice(PhysicalDeviceRHI physicalDevice);
    
};

using InstanceRHI = std::shared_ptr<Instance>;

} // namespace rhi
} // namespace bl