#pragma once

#include "VulkanBuffer.h"

namespace bl {

/**
 * @brief Allocates a vulkan buffer with a size of frameCount * size, uses dynamic offsets.
 * Always mapped, these buffers are supposed to be extremely quick.
 * Uses a staging buffer if the buffer is not host coherent.
 */
class VulkanBufferFrameRing
{
    uint32_t frameCount;
    uint32_t sizePerFrame;
    VulkanBuffer _buffer;
    bool usesStagingBuffer;
    VulkanBuffer _stagingBuffer;

public:
    VulkanBufferFrameRing(VulkanDevice* device, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, uint32_t frameCount, VkDeviceSize size);
    ~VulkanBufferFrameRing();

    uint32_t GetFrameCount();
    uint32_t GetDynamicOffset(uint32_t currentFrame);
    uint32_t GetWholeSize();

    VulkanBufferFrameRing& operator=(const VulkanBufferFrameRing& rhs) = delete;
    VulkanBufferFrameRing& operator=(VulkanBufferFrameRing&& rhs) noexcept;

    VkDeviceSize GetSize() const override; /** @brief Returns the size of the buffer in bytes. */
    void Upload(std::span<std::byte> data, uint32_t currentFrame);
    void Flush(VkDeviceSize offset, VkDeviceSize size);
};

}