#include "Graphics/Mesh.h"
#include "Graphics/VulkanBuffer.h"
#include <cstddef>


namespace bl {

void StaticMesh::setIndices(const std::vector<uint32_t>& indices)
{
    // create the staging buffer
    size_t ibSize = sizeof(uint32_t) * indices.size();

    // copy staging buffer to a buffer on device
    _indexBuffer = VulkanBuffer{_device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, ibSize};
    _indexBuffer.Upload(bl::vector_as_bytes(indices));
    _indexCount = (uint32_t)indices.size();
}

void StaticMesh::bind(VkCommandBuffer cmd)
{
    VkDeviceSize offset = 0;
    VkBuffer buffer = _vertexBuffer.Get();
    vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &offset);
    vkCmdBindIndexBuffer(cmd, _indexBuffer.Get(), 0, VK_INDEX_TYPE_UINT32);
}

void StaticMesh::draw(VkCommandBuffer cmd)
{
    vkCmdDrawIndexed(cmd, _indexCount, 1, 0, 0, 0);
}

} // namespace bl