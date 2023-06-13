#include "Render/Mesh.hpp"

blMesh::blMesh(
    std::shared_ptr<blRenderDevice> renderDevice, 
    const std::vector<blVertex>& vertices, 
    const std::vector<uint32_t>& indices)
    : _vertexBuffer(_renderDevice, vk::BufferUsageFlagBits::eVertexBuffer, sizeof(blVertex) * vertices.size(), vertices.data())
    , _indexBuffer(_renderDevice, vk::BufferUsageFlagBits::eIndexBuffer, sizeof(uint32_t) * indices.size(), indices.data())
{   
}