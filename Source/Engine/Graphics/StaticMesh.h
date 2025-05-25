#pragma once

#include "VulkanBuffer.h"

namespace bl {

template<typename T>
std::span<const std::byte> vector_as_bytes(const std::vector<T>& vec) {
    const std::byte* data_ptr = reinterpret_cast<const std::byte*>(vec.data());
    size_t size = sizeof(T) * vec.size();
    return std::span<const std::byte>{data_ptr, size};
}

class StaticMesh 
{
public:
    template<typename TVertex>
    StaticMesh(VulkanDevice* device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices);

    template<typename TVertex>
    void SetVertices(const std::vector<TVertex>& vertices);
    void SetIndices(const std::vector<uint32_t>& indices);
    void Draw(VkCommandBuffer cmd, uint32_t numInstances=1);

private:
    VulkanDevice* _device;
    VulkanBuffer _vertexBuffer;
    VulkanBuffer _indexBuffer;
    uint32_t _indexCount;
    VulkanMaterialInstance* _material;
};

} // namespace bl

#include "StaticMesh.inl"
