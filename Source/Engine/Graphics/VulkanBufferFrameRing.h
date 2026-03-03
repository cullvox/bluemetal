#pragma once

#include <span>
#include <array>

#include "VulkanConfig.h"
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
    VkDeviceSize _bufferSize;
    std::array<VulkanBuffer, VulkanConfig::maxFramesInFlight> _buffers;
    std::array<void*, VulkanConfig::maxFramesInFlight> _mappings;

public:
    VulkanBufferFrameRing();
    VulkanBufferFrameRing(
        VulkanDevice*               device,
        VkBufferUsageFlags          usage,
        VmaMemoryUsage              memoryUsage,
        VkDeviceSize                size,
        VmaAllocationCreateFlags    flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    VulkanBufferFrameRing(VulkanBufferFrameRing& rhs) = delete;
    VulkanBufferFrameRing(VulkanBufferFrameRing&& rhs) noexcept;
    ~VulkanBufferFrameRing();

    VulkanBufferFrameRing& operator=(const VulkanBufferFrameRing& rhs) = delete;
    VulkanBufferFrameRing& operator=(VulkanBufferFrameRing&& rhs) noexcept;

    VkBuffer GetBuffer(uint32_t currentFrame) const;
    VkDescriptorBufferInfo GetDescriptorInfo(uint32_t currentFrame) const;

    void Upload(std::span<const std::byte> data, uint32_t currentFrame);
};

}