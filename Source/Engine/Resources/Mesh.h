#pragma once

#include "Resource.h"
#include "Texture2D.h"
#include "Precompiled.h"
#include "Graphics/Vertex.h"
#include "Graphics/VulkanBuffer.h"
#include "Graphics/GraphicsSystem.h"

namespace bl
{

class Mesh : public Resource
{
    GraphicsSystem* _system;
    VulkanBuffer _vertexBuffer;
    VulkanBuffer _indexBuffer;

    void UploadVertices(std::span<const std::byte> bytes);

public:
    Mesh(GraphicsSystem* system);
    Mesh(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path);
    ~Mesh() = default;

    template<VertexType T>
    void UploadVertices(std::span<T> vertices)
    {
        UploadVertices(std::as_bytes<T>(vertices));
    }

    void UploadIndices(std::span<uint32_t> indices);

    void Bind(VkCommandBuffer buffer);
};

}