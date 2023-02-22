#pragma once

//===========================//
#include <cstddef>
#include <cstdint>
//===========================//
#include "Render/RenderDevice.hpp"
//===========================//

class CBuffer
{
public:
    CBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, CRenderDevice* const pRenderDevice = g_pRenderDevice.get());
    ~CBuffer();

    void            Upload(size_t dataSize, const uint8_t* pData);
    VmaAllocation   GetAllocation() const noexcept;
    VkBuffer        GetBuffer() const noexcept;
    VkDeviceSize    GetSize() const noexcept;
private:
    RenderDevice& const     m_pRenderDevice;
    VmaAllocation           m_allocation;
    VkBuffer                m_buffer;
    VkDeviceSize            m_size;
};