#include "Mesh.h"
#include "Engine/Engine.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Vulkan.h"
#include "Graphics/VulkanMesh.h"

namespace bl {

Mesh::Mesh()
    : Resource()
{
    _system = GraphicsSystem::Get();
}

Mesh::Mesh(const std::filesystem::path& path)
    : Resource(path)
{
}

Mesh::~Mesh()
{
}

void Mesh::Release()
{
    Resource::Release();
    _mesh.reset();
}

void Mesh::Upload(std::span<const std::byte> vertices, std::span<uint32_t> indices)
{
    _mesh = std::make_unique<VulkanMesh>(_system->GetDevice(), vertices, indices);
}

void Mesh::Bind(VkCommandBuffer cmd)
{
    _mesh->Bind(cmd);
}

VulkanMesh* Mesh::GetMesh()
{
    return _mesh.get();
}

uint32_t Mesh::GetIndicesCount()
{
    return _mesh->GetIndicesCount();
}

}