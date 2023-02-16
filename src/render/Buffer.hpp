#pragma once

#include "RenderDevice.hpp"
#include "vulkan/vulkan_core.h"

class CBuffer
{
public:
    CBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, CRenderDevice* const pRenderDevice = g_pRenderDevice.get());
    ~CBuffer();

    void Upload();

    VmaAllocation   GetAllocation() const noexcept;
    VkBuffer        GetBuffer() const noexcept;
    VkDeviceSize    GetSize() const noexcept;

private:
    CRenderDevice* const m_pRenderDevice;
    VmaAllocation   m_allocation;
    VkBuffer        m_buffer;
    VkDeviceSize    m_size;
};