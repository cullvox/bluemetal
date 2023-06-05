#include "Render/Mesh.hpp"
#include "Render/Buffer.hpp"
#include "Render/Vertex.hpp"

blMesh::blMesh(
    std::shared_ptr<blRenderDevice>     renderDevice, 
    const std::vector<blVertex>&        vertices, 
    const std::vector<uint32_t>&        indices)
{

    size_t vertexBufferSize = sizeof(blVertex) * vertices.count();

    blBuffer vertexBuffer(renderDevice, vertexBufferSize, VK_BUFFER_)

}