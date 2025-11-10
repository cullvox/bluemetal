#pragma once

#include "VulkanBuffer.h"

class VulkanPerFrameBuffer {
    VulkanBuffer _buffer;
    size_t _sizePerFrame;
public:
    VulkanPerFrameBuffer() = default;
    VulkanPerFrameBuffer(VulkanDevice* device, size_t sizePerFrame, uint32_t numFrames);
    ~VulkanPerFrameBuffer();
    VulkanPerFrameBuffer(const VulkanPerFrameBuffer&) = delete;
    VulkanPerFrameBuffer(VulkanPerFrameBuffer&&) = default;
    VulkanPerFrameBuffer& operator=(const VulkanPerFrameBuffer&) = delete;
    VulkanPerFrameBuffer& operator=(VulkanPerFrameBuffer&&) = default;

    void Upload(uint32_t frameIndex, std::span<const std::byte> data);
    VkBuffer GetBuffer() const;
    size_t GetSizePerFrame() const;
};