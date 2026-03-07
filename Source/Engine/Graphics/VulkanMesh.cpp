#include "VulkanMesh.h"

namespace bl
{

VulkanMesh::VulkanMesh()
{
    _device = nullptr;
    _vertexBuffer = {};
    _indexBuffer = {};
    _vertexCount = 0;
    _indexCount = 0;
    _hasIndices = false;
}

VulkanMesh::VulkanMesh(VulkanDevice* device, std::span<const std::byte> vertices, uint32_t vertexCount)
    : _device(device)
{
    _vertexBuffer = VulkanBuffer { _device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, vertices.size_bytes(), nullptr, false };
    _vertexBuffer.Upload(std::as_bytes(vertices));
    _vertexCount = vertexCount;
    _indexCount = 0;
    _hasIndices = false;
}

VulkanMesh::VulkanMesh(VulkanDevice* device, std::span<const std::byte> vertices, std::span<const uint32_t> indices)
    : _device(device)
{
    _vertexBuffer = VulkanBuffer { _device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, vertices.size_bytes(), nullptr, false };
    _vertexBuffer.Upload(std::as_bytes(vertices));
    _vertexCount = 0;

    _hasIndices = true;
    _indexBuffer = VulkanBuffer { _device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, indices.size_bytes(), nullptr, false };
    _indexBuffer.Upload(std::as_bytes(indices));
    _indexCount = static_cast<uint32_t>(indices.size());
}

VulkanMesh::VulkanMesh(VulkanMesh&& rhs) noexcept
{
    _device = rhs._device;
    _vertexBuffer = std::move(rhs._vertexBuffer);
    _indexBuffer = std::move(rhs._indexBuffer);
    _vertexCount = rhs._vertexCount;
    _indexCount = rhs._indexCount;
    _hasIndices = rhs._hasIndices;

    rhs._device = {};
    rhs._vertexBuffer = {};
    rhs._indexBuffer = {};
    rhs._vertexCount = {};
    rhs._indexCount = {};
    rhs._hasIndices = {};
}

VulkanMesh::~VulkanMesh()
{
}

VulkanMesh& VulkanMesh::operator=(VulkanMesh&& rhs) noexcept
{
    _device = rhs._device;
    _vertexBuffer = std::move(rhs._vertexBuffer);
    _indexBuffer = std::move(rhs._indexBuffer);
    _vertexCount = rhs._vertexCount;
    _indexCount = rhs._indexCount;
    _hasIndices = rhs._hasIndices;

    rhs._device = {};
    rhs._vertexBuffer = {};
    rhs._indexBuffer = {};
    rhs._vertexCount = {};
    rhs._indexCount = {};
    rhs._hasIndices = {};
    return *this;
}

VulkanBuffer& VulkanMesh::GetVertexBuffer()
{
    return _vertexBuffer;
}

VulkanBuffer& VulkanMesh::GetIndexBuffer()
{
    return _indexBuffer;
}

uint32_t VulkanMesh::GetVerticesCount() const
{
    return _vertexCount;
}

uint32_t VulkanMesh::GetIndicesCount() const
{
    return _indexCount;
}

bool VulkanMesh::UseIndexBuffer() const
{
    return _hasIndices;
}


void VulkanMesh::Bind(VkCommandBuffer cmd) const
{
    VkDeviceSize offset = 0;
    VkBuffer buffer = _vertexBuffer.Get();
    vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &offset);

    if (_hasIndices)
    {
        vkCmdBindIndexBuffer(cmd, _indexBuffer.Get(), 0, VK_INDEX_TYPE_UINT32);
    }
}

void VulkanMesh::Draw(VkCommandBuffer cmd, uint32_t instanceCount, uint32_t firstInstance) const
{
    VkDeviceSize offset = 0;
    VkBuffer buffer = _vertexBuffer.Get();
    vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &offset);

    if (_hasIndices)
    {
        vkCmdBindIndexBuffer(cmd, _indexBuffer.Get(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDraw(cmd, _vertexCount, instanceCount, 0, firstInstance);
    }
    else
    {
        vkCmdDrawIndexed(cmd, _indexCount, instanceCount, 0, 0, firstInstance);
    }
}

} // namespace bl