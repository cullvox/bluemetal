#pragma once


#include <cstdint>
#include <span>

#include "Graphics/Vertex.h"
#include "Resource.h"

namespace bl {

class GraphicsSystem;
class VulkanMesh;

class Mesh : public Resource {
    GraphicsSystem* _system;
    std::unique_ptr<VulkanMesh> _mesh;
    void Upload(std::span<const std::byte> bytes, std::span<uint32_t> indices);

public:
    Mesh(Engine& engine, const std::filesystem::path& path);
    ~Mesh();

    template <typename T>
    void Upload(std::span<T> vertices, std::span<uint32_t> indices)
    {
        Upload(std::as_bytes<T>(vertices), indices);
    }

    VulkanMesh* GetMesh();
    uint32_t GetIndicesCount();

    void Bind(VkCommandBuffer buffer);
};

}