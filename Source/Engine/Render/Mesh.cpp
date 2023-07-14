#include "Render/Mesh.hpp"

blMesh::blMesh(std::shared_ptr<blRenderDevice> renderDevice, const std::vector<blVertex>& vertices, const std::vector<uint32_t>& indices)
{   

    // Create the vertex and index buffers
    size_t vbSize = sizeof(blVertex) * vertices.size();
    size_t ibSize = sizeof(uint32_t) * indices.size();

    _vertexBuffer = std::make_shared<blBuffer>(_renderDevice, vk::BufferUsageFlagBits::eVertexBuffer, vbSize, vertices.data());
    _indexBuffer = std::make_shared<blBuffer>(_renderDevice, vk::BufferUsageFlagBits::eIndexBuffer, ibSize, indices.data());


}