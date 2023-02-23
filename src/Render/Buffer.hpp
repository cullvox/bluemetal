#pragma once

//===========================//
#include <cstddef>
#include <cstdint>
//===========================//
#include "Render/RenderDevice.hpp"
//===========================//

namespace bl
{

class Buffer
{
public:
    Buffer(RenderDevice& renderDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
    ~Buffer();

    void            Upload(size_t dataSize, const uint8_t* pData);
    VmaAllocation   GetAllocation() const noexcept;
    VkBuffer        GetBuffer() const noexcept;
    VkDeviceSize    GetSize() const noexcept;
private:
    RenderDevice&   m_renderDevice;
    VmaAllocation   m_allocation;
    VkBuffer        m_buffer;
    VkDeviceSize    m_size;
};

} /* namespace bl */