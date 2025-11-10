#pragma once

#include "Precompiled.h"

#include <nlohmann/json.hpp>

#include "Vertex.h"
#include "Vulkan.h"
#include "VulkanConversions.h"
#include "VulkanDescriptorSetLayoutCache.h"
#include "VulkanPipelineLayoutCache.h"
#include "VulkanReflectedDescriptorSet.h"
#include "VulkanReflectedPushConstant.h"

struct SpvReflectBlockVariable;

namespace bl {

class VulkanDevice;
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
        VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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
        std::vector<VkPipelineColorBlendAttachmentState> attachments = { {
            .blendEnable = VK_TRUE,
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

/**
 * @class VulkanReflectedPipeline
 * @brief Reflection data for a Vulkan pipeline.
 */
struct VulkanReflectedPipeline {
public:
    /**
     * @brief Gets the reflected descriptor sets of this pipeline.
     * @return A map of descriptor set index to reflected descriptor set.
     *
     * This map contains all the descriptor sets that were reflected from the shaders.
     * It allows the user to query the descriptor sets and their bindings.
     *
     * You can also modify the bindings in this map to change how the pipeline will use them.
     * This is useful for when creating materials for changing uniform buffers to dynamic uniform buffers.
     * Doing so doesn't affect how shaders interpret the data, but it allows for more efficient memory usage.
     */
    static VulkanReflectedPipeline Reflect(const VulkanPipelineStateInfo::Stages& stages);

    std::map<uint32_t, VulkanReflectedDescriptorSet> descriptorSetMetadata;
    std::vector<VulkanReflectedPushConstant> pushConstantMetadata;
};

/**
 * @class VulkanPipeline
 * @brief Represents a Vulkan graphics pipeline.
 *
 * This class encapsulates the creation and management of a Vulkan graphics pipeline,
 * including shader stages, descriptor set layouts, push constants and states.
 */
class VulkanPipeline {
    VulkanDevice* _device;
    VulkanReflectedPipeline _reflection;
    VkPipelineLayout _layout;
    VkPipeline _pipeline;
    std::map<uint32_t, VkDescriptorSetLayout> _descriptorSetLayouts;

public:
    VulkanPipeline() = default;

    /**
     * @brief Constructs a Vulkan pipeline object.
     * @param device Vulkan device to create the pipeline with.
     * @param info Pipeline state info, how to render.
     * @param renderPass The render pass this pipeline is running on.
     * @param subpass What subpass this render pass is running on.
     * @param reflection Information about how descriptor sets interact with the pipeline.
     *        Can be nullptr if the user doesn't want to manually preform reflection.
     */
    VulkanPipeline(
        VulkanDevice* device,
        const VulkanPipelineStateInfo& info,
        VkRenderPass renderPass,
        uint32_t subpass,
        const VulkanReflectedPipeline* reflection = nullptr);

    /**
     * @brief Move constructor for VulkanPipeline.
     * @param move The VulkanPipeline to move from.
     */
    VulkanPipeline(VulkanPipeline&& move) noexcept;

    /**
     * @brief Destructor for VulkanPipeline.
     * @details This will destroy the pipeline and all resources associated with it.
     */
    ~VulkanPipeline();

    /**
     * @brief Move assignment operator for VulkanPipeline.
     * @param move The VulkanPipeline to move from.
     * @return Reference to this VulkanPipeline.
     */
    VulkanPipeline& operator=(VulkanPipeline&& move) noexcept;

    /**
     * @brief Gets the reflection data of this pipeline.
     * @return The reflection data of this pipeline.
     */
    const VulkanReflectedPipeline& GetReflection() const;

    /**
     * @brief Gets the pipeline layout used by this pipeline.
     * @return The Vulkan pipeline layout used by this pipeline.
     */
    VkPipelineLayout GetPipelineLayout() const;

    /**
     * @brief Gets the Vulkan pipeline handle.
     * @return The Vulkan pipeline handle.
     */
    VkPipeline GetPipeline() const;

    /**
     * @brief Gets the descriptor set layouts used by this pipeline.
     * @return A map of descriptor set index to descriptor set layout.
     *
     * This map contains all the descriptor set layouts that were used for the pipeline.
     * It allows the user to query the descriptor set layouts and their bindings.
     */
    const std::map<uint32_t, VkDescriptorSetLayout>& GetDescriptorSetLayouts() const;
};

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VulkanPipelineStateInfo::Stages, shaders)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VulkanPipelineStateInfo::VertexState, inputBindings, inputAttribs, topology, primitiveRestartEnable)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VulkanPipelineStateInfo::RasterizerState, depthClampEnable, rasterizerDiscardEnable, polygonMode, cullMode, frontFace, depthBiasEnable, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor, lineWidth)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VulkanPipelineStateInfo::MultisampleState, rasterizationSamples, sampleShadingEnable, minSampleShading, alphaToCoverageEnable, alphaToOneEnable)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VulkanPipelineStateInfo::DepthStencilState, depthTestEnable, depthWriteEnable, depthCompareOp, depthBoundsTestEnable, stencilTestEnable, front, back, minDepthBounds, maxDepthBounds)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VulkanPipelineStateInfo::ColorBlendState, logicOpEnable, logicOp, attachments, blendConstants)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VulkanPipelineStateInfo, vertexState, rasterizerState, multisampleState, depthStencilState, colorBlendState, dynamicStates)

} // namespace bl

/* Getting realistic about descriptor sets.

    Descriptor Slot: 0
        Reserved for global data of the frame.

        This Includes
            * Current Time
            * Camera MVP
            * Resolution
            * Texture Atlas
            * ... (TBD)

    Descriptor Slot: 1
        Reserved for material data. Will change multiple times per frame. Each
        material instance's data will be uploaded here.

        Material data is custom to the shader and pipelines will agree to what
        ever the shader says because of reflection. The material system will
        also take from the pipelines reflection and build a list of values that
        can be changed at runtime using this descriptor index.

        Pipelines are essentially here on the order.

    Descriptor Slot: 2
        Reserved for object data. This will change many times during the runtime
        of a singe frame.

        This Includes:
            * Object/Mesh Transform
            * ... (TBD)

    Descriptor Slot: 3
        To be determined, might be for instanced data and/or textures?
        Instanced data might need to go on slot 2.

*/
