#pragma once

#include "Device.h"

namespace bl
{

class BLUEMETAL_API Buffer
{
public:
    Buffer(std::shared_ptr<Device> device, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, VmaAllocationInfo* pInfo = nullptr, bool mapped = false);
    ~Buffer();

    VkDeviceSize getSize();
    VkBuffer getBuffer();
    VmaAllocation getAllocation();

private:
    std::shared_ptr<Device> _device;
    VkDeviceSize            _size;
    VkBuffer                _buffer;
    VmaAllocation           _allocation;
};

} // namespace bl