#include "Graphics/Mesh.h"

namespace bl {

void Mesh::setIndices(const std::vector<uint32_t>& indices)
{
    // create the staging buffer
    size_t ibSize = sizeof(uint32_t) * indices.size();

    // copy staging buffer to a buffer on device
    _indexBuffer = std::make_unique<GfxBuffer>(_device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ibSize);
    _indexBuffer->upload((void*)indices.data());
}

void Mesh::bind(VkCommandBuffer cmd)
{
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &_vertexBuffer->getBuffer(), &offset);
}

} // namespace bl