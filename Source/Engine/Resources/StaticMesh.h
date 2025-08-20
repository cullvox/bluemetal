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

} // namespace bl

#include "StaticMesh.inl"
