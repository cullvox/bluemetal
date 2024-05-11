#include "Graphics/Mesh.h"

namespace bl {

void Mesh::setIndices(const std::vector<uint32_t>& indices)
{
    // create the staging buffer
    size_t ibSize = sizeof(uint32_t) * indices.size();

    // copy staging buffer to a buffer on device
    _indexBuffer = std::make_unique<Buffer>(_device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);
    _indexBuffer->Upload((void*)indices.data());
    _indexCount = (uint32_t)indices.size();
}

void Mesh::bind(VkCommandBuffer cmd)
{
    VkDeviceSize offset = 0;
    VkBuffer buffer = _vertexBuffer->Get();
    vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &offset);
    vkCmdBindIndexBuffer(cmd, _indexBuffer->Get(), 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::draw(VkCommandBuffer cmd)
{
    vkCmdDrawIndexed(cmd, _indexCount, 1, 0, 0, 0);
}

} // namespace bl