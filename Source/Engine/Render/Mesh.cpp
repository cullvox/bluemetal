#include "Render/Mesh.h"

void blMesh::setIndices(const std::vector<uint32_t>& indices)
{
     // create the staging buffer
    size_t ibSize = sizeof(uint32_t) * indices.size();
    
    VmaAllocationInfo allocInfo = {};
    blBuffer stagingBuffer(_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, ibSize, &allocInfo, true);

    // copy the indices to the mapped ptr visible to host
    memcpy(allocInfo.pMappedData, indices.data(), ibSize);

    // copy staging buffer to a buffer on device
    _indexBuffer = std::make_unique<blBuffer>(_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);

    _device->immediateSubmit(
        [&](VkCommandBuffer cmd)
        {
            VkBufferCopy region = { 0, 0, ibSize };
            vkCmdCopyBuffer(cmd, stagingBuffer.getBuffer(), _vertexBuffer->getBuffer(), 1, &region);
        });
}