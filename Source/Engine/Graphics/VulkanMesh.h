#pragma once

#include <span>

#include "Vertex.h"
#include "VulkanBuffer.h"

namespace bl {

class VulkanMesh
{
    VulkanBuffer _vertexBuffer;
    VulkanBuffer _indexBuffer;
    uint32_t _indicesCount;

public:
    VulkanMesh(std::span<Vertex> vertices, std::span<uint32_t> indices);
    ~VulkanMesh();

}

} // namespace bl