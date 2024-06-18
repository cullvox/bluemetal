#pragma once

#include "VulkanBuffer.h"

namespace bl {

class Mesh {
public:
    template<typename TVertex>
    Mesh(VulkanDevice* device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices);

    template<typename TVertex>
    void setVertices(const std::vector<TVertex>& vertices);
    void setIndices(const std::vector<uint32_t>& indices);

    void bind(VkCommandBuffer cmd);
    void draw(VkCommandBuffer cmd);

private:
    VulkanDevice* _device;
    std::unique_ptr<Buffer> _vertexBuffer;
    std::unique_ptr<Buffer> _indexBuffer;
    uint32_t _indexCount;
};

} // namespace bl

#include "Mesh.inl"
