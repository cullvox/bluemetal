#include "Mesh.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Vulkan.h"

namespace bl {

Mesh::Mesh(GraphicsSystem* system)
    : Resource(nullptr, system, "")
    , _system(system)
{
}

Mesh::Mesh(ResourceSystem& resourceSystem, GraphicsSystem* system, const std::filesystem::path& path)
    : Resource(resourceSystem, system, path)
    , _system(system)
{
}

Mesh::~Mesh()
{
}

void Mesh::UploadVertices(std::span<const std::byte> bytes)
{
    _vertexBuffer = VulkanBuffer { _system->GetDevice(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, bytes.size(), nullptr, false };
    _vertexBuffer.Upload(bytes);
}

void Mesh::UploadIndices(std::span<uint32_t> indices)
{
    _indexBuffer = VulkanBuffer { _system->GetDevice(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, indices.size_bytes(), nullptr, false };
    _indexBuffer.Upload(std::as_bytes(indices));
    _indicesCount = static_cast<uint32_t>(indices.size());
}

void Mesh::Bind(VkCommandBuffer cmd)
{
    VkBuffer buffer = _vertexBuffer.Get();
    std::array<VkDeviceSize, 1> offsets { { 0 } };
    vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, offsets.data());
    vkCmdBindIndexBuffer(cmd, _indexBuffer.Get(), 0, VK_INDEX_TYPE_UINT32);
}

uint32_t Mesh::GetIndicesCount()
{
    return _indicesCount;
}

}