#pragma once

#include "Graphics/VulkanBuffer.h"
#include "Graphics/Vertex.h"

namespace bl {

/**
 * @class StaticMesh
 * @brief Represents a static mesh with vertex and index buffers.
 *
 * This class is used to create and manage static meshes in Vulkan.
 * It provides methods to set vertices and indices, bind the mesh for rendering,
 * and draw the mesh.
 */
class StaticMesh 
{
public:
    template<VertexType TVertex>
    StaticMesh(VulkanDevice* device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices);

    template<typename TVertex>
    void SetVertices(const std::vector<TVertex>& vertices);
    void SetIndices(const std::vector<uint32_t>& indices);
    void Bind(VkCommandBuffer cmd);
    void Draw(VkCommandBuffer cmd, uint32_t numInstances=1);

private:
    VulkanDevice* _device;
    VulkanBuffer _vertexBuffer;
    VulkanBuffer _indexBuffer;
    uint32_t _indexCount;
};

template<typename T>
std::span<const std::byte> VectorAsBytes(const std::vector<T>& vec) {
    const std::byte* data_ptr = reinterpret_cast<const std::byte*>(vec.data());
    size_t size = sizeof(T) * vec.size();
    return std::span<const std::byte>{data_ptr, size};
}

template<VertexType TVertex>
StaticMesh::StaticMesh(VulkanDevice* device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices)
    : _device(device)
{
    SetVertices(vertices);
    SetIndices(indices);
}

template<typename TVertex>
void StaticMesh::SetVertices(const std::vector<TVertex>& vertices)
{
    // Create the vertex and index buffers
    size_t vbSize = sizeof(TVertex) * vertices.size();

    _vertexBuffer = VulkanBuffer{_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, vbSize};
    _vertexBuffer.Upload(bl::VectorAsBytes(vertices));
}

} // namespace bl
