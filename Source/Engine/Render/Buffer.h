#pragma once

#include "Device.h"

class BLUEMETAL_API blBuffer
{
public:
    blBuffer(std::shared_ptr<blDevice> renderDevice, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size, VmaAllocationInfo* pInfo = nullptr, bool mapped = false);
    ~blBuffer();

    VkDeviceSize getSize();
    VkBuffer getBuffer();
    VmaAllocation getAllocation();

private:
    std::shared_ptr<blDevice>       _device;
    VkDeviceSize                    _size;
    VkBuffer                        _buffer;
    VmaAllocation                   _allocation;
};