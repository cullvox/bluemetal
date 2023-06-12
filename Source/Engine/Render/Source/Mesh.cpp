#include "Render/Mesh.hpp"

blMesh::blMesh(
    std::shared_ptr<blRenderDevice> renderDevice, 
    const std::vector<blVertex>& vertices, 
    const std::vector<uint32_t>& indices)
{

    size_t vertexBufferSize = sizeof(blVertex) * vertices.size();
    size_t indexBufferSize = sizeof(uint32_t) * indices.size(); 

    _vertexBuffer = blBuffer(_renderDevice, vk::BufferUsageFlagBits::eVertexBuffer, vertexBufferSize, vertices.data());
    _indexBuffer = blBuffer(_renderDevice, vk::BufferUsageFlagBits::eIndexBuffer, indexBufferSize, indices.data());

    
}