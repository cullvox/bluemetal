#pragma once

#include "Core/Macros.hpp"
#include "Render/RenderDevice.hpp"
#include <vulkan/vulkan.hpp>

class BLUEMETAL_RENDER_API blBuffer
{
public:
    blBuffer( // creates a normal vulkan buffer
        std::shared_ptr<blRenderDevice> renderDevice, 
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags memoryProperties,
        vk::DeviceSize size,
        VmaAllocationInfo* BL_NULLABLE pInfo = nullptr,
        bool mapped = false); // will create a mapped vma allocation 
    blBuffer( // + automatically uploads cpu -> gpu
        std::shared_ptr<blRenderDevice> renderDevice,
        vk::BufferUsageFlags usage,
        vk::DeviceSize size, 
        const void* pData); 
    blBuffer(blBuffer& other) = delete;
    blBuffer(blBuffer&& other) noexcept;
    ~blBuffer() noexcept;

    blBuffer& operator=(blBuffer&& rhs) noexcept;

    vk::DeviceSize getSize() const noexcept;
    vk::Buffer getBuffer() const noexcept;
    VmaAllocation getAllocation() const noexcept;

private:
    void collapse() noexcept;

    std::shared_ptr<blRenderDevice> _renderDevice;

    vk::DeviceSize    _size;
    vk::Buffer        _buffer;
    VmaAllocation   _allocation;
};