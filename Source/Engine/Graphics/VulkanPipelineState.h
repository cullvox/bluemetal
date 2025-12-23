#pragma once

#include <vector>
#include <array>

#include "Vertex.h"
#include "VulkanForward.h"

namespace bl {

class VulkanShader;

/**
 * @brief Creation info for Vulkan pipeline objects.
 */
struct VulkanPipelineStateInfo {

    /**
     * @brief Stages of the pipeline.
     */
    struct Stages {
        std::vector<VulkanShader*> shaders;
    } stages;

    /**
     * @brief Vertex input state for the pipeline.
     */
    struct VertexState {
        std::vector<VkVertexInputBindingDescription> inputBindings = Vertex::GetBindingDescriptions();
        std::vector<VkVertexInputAttributeDescription> inputAttribs = Vertex::GetBindingAttributeDescriptions();
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        bool primitiveRestartEnable = VK_FALSE;
    } vertexState;

    /**
     * @brief Input assembly state for the pipeline.
     */
    struct RasterizerState {
        VkBool32 depthClampEnable = VK_FALSE;
        VkBool32 rasterizerDiscardEnable = VK_FALSE;
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
        VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
        VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        VkBool32 depthBiasEnable = VK_FALSE;
        float depthBiasConstantFactor = 0.0f;
        float depthBiasClamp = 0.0f;
        float depthBiasSlopeFactor = 0.0f;
        float lineWidth = 1.0f;
    } rasterizerState;

    /**
     * @brief Multisample state for the pipeline.
     */
    struct MultisampleState {
        VkBool32 sampleShadingEnable = VK_FALSE;
        float minSampleShading = 0.2f;
        VkBool32 alphaToCoverageEnable = VK_FALSE;
        VkBool32 alphaToOneEnable = VK_FALSE;
    } multisampleState;

    /**
     * @brief Depth and stencil state for the pipeline.
     */
    struct DepthStencilState {
        VkBool32 depthTestEnable = VK_TRUE;
        VkBool32 depthWriteEnable = VK_TRUE;
        VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        VkBool32 depthBoundsTestEnable = VK_FALSE;
        VkBool32 stencilTestEnable = VK_FALSE;
        VkStencilOpState front = {};
        VkStencilOpState back = {};
        float minDepthBounds = 0.0f;
        float maxDepthBounds = 1.0f;
    } depthStencilState;

    /**
     * @brief Color blend state for the pipeline.
     */
    struct ColorBlendState {
        VkBool32 logicOpEnable = VK_FALSE;
        VkLogicOp logicOp = VK_LOGIC_OP_COPY;
        std::vector<VkPipelineColorBlendAttachmentState> attachments = { { .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT } };
        std::array<float, 4> blendConstants = {
            0.0f, 0.0f, 0.0f, 0.0f
        };
    } colorBlendState;

    /**
     * @brief Dynamic states for the pipeline.
     */
    std::vector<VkDynamicState> dynamicStates;
};

} // namespace bl