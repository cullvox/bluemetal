#pragma once

#include <span>

#include "Vertex.h"
#include "VulkanBuffer.h"

namespace bl {

class VulkanMesh
{
    VulkanDevice* _device;
    VulkanBuffer _vertexBuffer;
    VulkanBuffer _indexBuffer;
    uint32_t _vertexCount;
    uint32_t _indexCount;
    bool _hasIndices;

public:
    VulkanMesh();
    VulkanMesh(VulkanMesh&& rhs) noexcept;
    VulkanMesh(VulkanDevice* device, std::span<const std::byte> vertices, uint32_t vertexCount);
    VulkanMesh(VulkanDevice* device, std::span<const std::byte> vertices, std::span<const uint32_t> indices);
    ~VulkanMesh();

    VulkanMesh& operator=(VulkanMesh&& rhs) noexcept;

    VulkanBuffer& GetVertexBuffer();
    VulkanBuffer& GetIndexBuffer();
    bool UseIndexBuffer() const;
    uint32_t GetIndicesCount() const;
    uint32_t GetVerticesCount() const;

    void Bind(VkCommandBuffer cmd) const;
    void Draw(VkCommandBuffer cmd, uint32_t instanceCount = 1, uint32_t firstInstance = 0) const;
};

} // namespace bl