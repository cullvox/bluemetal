#pragma once

#include "Math/Math.h"
#include "Vulkan.h"

namespace bl
{

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> descriptions = {
            { 0, sizeof(bl::Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
        };
        return descriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> GetBindingAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributes = {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
            {2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords)},
        };
        return attributes;
    }
};

struct VoxelVertex
{
    uint32_t v;
};

} // namespace bl