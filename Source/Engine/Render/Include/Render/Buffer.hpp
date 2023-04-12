#pragma once

#include "Render/RenderDevice.hpp"

class BLOODLUST_RENDER_API blBuffer
{
public:
    blBuffer(const blRenderDevice* renderDevice, VkDeviceSize size, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
    ~blBuffer() noexcept;

    blBuffer& operator=(blBuffer&& rhs) noexcept;
    blBuffer& operator=(blBuffer& rhs);

    VkDeviceSize getSize() const noexcept;
    VmaAllocation getAllocation() const noexcept;
    VkBuffer getBuffer() const noexcept;
    void upload(size_t size, const uint8_t* pData);

private:
    const blRenderDevice* _pRenderDevice;

    VkDeviceSize    _size;
    VmaAllocation   _allocation;
    VkBuffer        _buffer;
};