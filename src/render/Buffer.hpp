#pragma once

#include "RenderDevice.hpp"

class CBuffer
{
public:
    CBuffer(CRenderDevice* pRenderDevice = g_pRenderDevice.get());
    ~CBuffer();

    VmaAllocation getAllocation() const noexcept;
    VkBuffer getBuffer() const noexcept;

private:
    VmaAllocation m_allocation;
    VkBuffer m_buffer;
};