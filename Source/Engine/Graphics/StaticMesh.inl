
namespace bl
{

template<typename TVertex>
StaticMesh::StaticMesh(VulkanDevice* device, const std::vector<TVertex>& vertices, const std::vector<uint32_t>& indices)
    : _device(device)
{
    SetVertices(vertices);
    SetIndices(indices);
}

template<typename TVertex>
void StaticMesh::SetVertices(const std::vector<TVertex>& vertices)
{
    // Create the vertex and index buffers
    size_t vbSize = sizeof(TVertex) * vertices.size();

    _vertexBuffer = VulkanBuffer{_device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, vbSize};
    _vertexBuffer.Upload(bl::vector_as_bytes(vertices));
}

} // namespace bl