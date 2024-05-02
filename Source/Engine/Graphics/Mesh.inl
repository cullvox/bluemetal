
namespace bl
{

template<typename TVertex>
Mesh::Mesh(Device* device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices)
    : _device(device)
{
    setVertices(vertices);
    setIndices(indices);
}
    

template<typename TVertex>
void Mesh::setVertices(const std::vector<TVertex>& vertices)
{
    // Create the vertex and index buffers
    size_t vbSize = sizeof(TVertex) * vertices.size();
    
    _vertexBuffer = std::make_unique<Buffer>(_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vbSize);
    _vertexBuffer->Upload((void*)vertices.data());
}

} // namespace bl