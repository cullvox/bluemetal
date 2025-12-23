#pragma once

#include <span>

#include "VulkanBuffer.h"

namespace bl {

/**
 * @brief Allocates a vulkan buffer with a size of frameCount * size, uses dynamic offsets.
 * Always mapped, these buffers are supposed to be extremely quick.
 * Uses a staging buffer if the buffer is not host coherent.
 * 
 * Due to alignment the buffer per frame data may be really be slightly bigger.
 */
class VulkanBufferFrameRing
{
    uint32_t _frameCount;
    VkDeviceSize _frameSize;
    VkDeviceSize _alignedFrameSize;
    VkDeviceSize _bufferWholeSize;
    VulkanBuffer _buffer;
    bool _usesStagingBuffer;
    VulkanBuffer _stagingBuffer;
    void* _mapped;

public:
    VulkanBufferFrameRing();
    VulkanBufferFrameRing(VulkanDevice* device, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, uint32_t frameCount, VkDeviceSize frameSize, bool mapped);
    VulkanBufferFrameRing(VulkanBufferFrameRing& rhs) = delete;
    VulkanBufferFrameRing(VulkanBufferFrameRing&& rhs) noexcept;
    ~VulkanBufferFrameRing();

    VulkanBufferFrameRing& operator=(const VulkanBufferFrameRing& rhs) = delete;
    VulkanBufferFrameRing& operator=(VulkanBufferFrameRing&& rhs) noexcept;

    uint32_t GetFrameCount() const;
    uint32_t GetDynamicOffset(uint32_t currentFrame) const;
    VkDeviceSize GetWholeSize() const;
    VkBuffer GetBuffer() const;

    void Upload(VkCommandBuffer cmd, std::span<const std::byte> data, uint32_t currentFrame);
};

}