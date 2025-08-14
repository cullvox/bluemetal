
namespace bl
{

template<typename T>
std::span<const std::byte> VectorAsBytes(const std::vector<T>& vec) {
    const std::byte* data_ptr = reinterpret_cast<const std::byte*>(vec.data());
    size_t size = sizeof(T) * vec.size();
    return std::span<const std::byte>{data_ptr, size};
}

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
    _vertexBuffer.Upload(bl::VectorAsBytes(vertices));
}

} // namespace bl