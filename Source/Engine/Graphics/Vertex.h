#pragma once

#include "Math/Math.h"
#include "Vulkan.h"

namespace bl {

template <typename T>
concept VertexType = requires(T) {
    { T::GetBindingDescriptions() } -> std::same_as<std::vector<VkVertexInputBindingDescription>>;
    { T::GetBindingAttributeDescriptions() } -> std::same_as<std::vector<VkVertexInputAttributeDescription>>;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangents;
    glm::vec2 texCoords;

    static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> descriptions = {
            { 0, sizeof(bl::Vertex), VK_VERTEX_INPUT_RATE_VERTEX }
        };
        return descriptions;
    }

    static std::vector<VkVertexInputAttributeDescription> GetBindingAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributes = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) },
            { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) },
            { 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangents) },
            { 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords) },
        };
        return attributes;
    }
};

struct VertexSkinned {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangents;
    glm::vec2 texCoords;
    glm::uvec4 boneIndices;
    glm::vec4 boneWeights;

    constexpr static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> descriptions = {
            { 0, sizeof(bl::VertexSkinned), VK_VERTEX_INPUT_RATE_VERTEX }
        };
        return descriptions;
    }

    constexpr static std::vector<VkVertexInputAttributeDescription> GetBindingAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributes = {
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexSkinned, position) },
            { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexSkinned, normal) },
            { 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexSkinned, tangents) },
            { 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexSkinned, texCoords) },
            { 4, 0, VK_FORMAT_R32G32B32A32_UINT, offsetof(VertexSkinned, boneIndices) },
            { 5, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexSkinned, boneWeights) },
        };
        return attributes;
    }
};

} // namespace bl