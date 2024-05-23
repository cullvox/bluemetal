#pragma once

#include "Precompiled.h"
#include "Vulkan.h"

namespace bl
{

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription description = {};
        description.binding = 0;
        description.stride = sizeof(bl::Vertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return description;
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