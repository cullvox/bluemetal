#pragma once

#include "Vertex.h"
#include "VulkanMutable.h"
#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanReflectedBlock.h"
#include "VulkanReflectedDescriptorSet.h"
#include "VulkanReflectedPushConstant.h"
#include "VulkanDescriptorSetLayoutCache.h"
#include "VulkanPipelineLayoutCache.h"

namespace bl {

/** @brief Create info for pipeline objects. */
struct VulkanPipelineStateInfo {

    struct Stages {
        std::vector<ResourceRef<VulkanShader>> shaders;
    } stages;

    struct VertexState {
        std::vector<VkVertexInputBindingDescription> vertexInputBindings = Vertex::GetBindingDescriptions();
        std::vector<VkVertexInputAttributeDescription> vertexInputAttribs = Vertex::GetBindingAttributeDescriptions();
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        bool primitiveRestartEnable = VK_FALSE;
    } vertexState;

    struct RasterizerState {
        VkBool32 depthClampEnable = VK_FALSE;
        VkBool32 rasterizerDiscardEnable = VK_FALSE;
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
        VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
        VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
        VkBool32 depthBiasEnable = VK_FALSE;
        float depthBiasConstantFactor = 0.0f;
        float depthBiasClamp = 0.0f;
        float depthBiasSlopeFactor = 0.0f;
        float lineWidth = 1.0f;
    } rasterizerState;
    
    struct MultisampleState {
        VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        VkBool32 sampleShadingEnable = VK_FALSE;
        float minSampleShading = 1.0f;
        const VkSampleMask* pSampleMask = nullptr;
        VkBool32 alphaToCoverageEnable = VK_FALSE;
        VkBool32 alphaToOneEnable = VK_FALSE;
    } multisampleState;

    struct DepthStencilState {
        VkBool32 depthTestEnable = VK_TRUE;
        VkBool32 depthWriteEnable = VK_TRUE;
        VkCompareOp depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
        VkBool32 depthBoundsTestEnable = VK_FALSE;
        VkBool32 stencilTestEnable = VK_FALSE;
        VkStencilOpState front = {};
        VkStencilOpState back = {};
        float minDepthBounds = 0.0f;
        float maxDepthBounds = 1.0f;
    };

    struct ColorBlendState {
        VkBool32 logicOpEnable = VK_FALSE;
        VkLogicOp logicOp = VK_LOGIC_OP_COPY;
        std::vector<VkPipelineColorBlendAttachmentState> attachments = {{
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        }};
        std::array<float, 4> blendConstants = {0.0f, 0.0f, 0.0f, 0.0f};
    } colorBlendState;

};

class VulkanReflectedPipeline {
public:

    /// @brief Reflection Constructor
    /// Organizes shader reflection data.
    VulkanReflectedPipeline() = default;
    VulkanReflectedPipeline(const VulkanPipelineStateInfo::Stages& stages);

    std::map<uint32_t, VulkanReflectedDescriptorSet>& GetReflectedDescriptorSets();
    const std::map<uint32_t, VulkanReflectedDescriptorSet>& GetReflectedDescriptorSets() const;
    std::vector<VulkanReflectedPushConstant>& GetReflectedPushConstants();
    const std::vector<VulkanReflectedPushConstant>& GetReflectedPushConstants() const;

private:
    void ReflectMembers(VulkanReflectedBlock& reflection, uint32_t binding, const SpvReflectBlockVariable& block, std::string parent = "");

    std::map<uint32_t, VulkanReflectedDescriptorSet> _descriptorSetMetadata;
    std::vector<VulkanReflectedPushConstant> _pushConstantMetadata;
};

/// @brief A program consisting of shaders designed to run on the GPU.
class VulkanPipeline : public VulkanMutable {
public:

    /// @brief Constructs a Vulkan pipeline object.
    /// @param[in] device Vukan device to create pipeline with.
    /// @param[in] info Pipeline state info, how to render.
    /// @param[in] renderPass The renderpass this pipeline is running on.
    /// @param[in] subpass What subpass this renderpass is running on.
    /// @param[in] reflection Information about how descriptor sets interact with the pipeline.
    ///             This is technically an immutable state since pipeline's will not be recompiled
    ///             at runtime for this.
    VulkanPipeline(
        VulkanDevice* device,
        VulkanDescriptorSetLayoutCache* descriptorSetLayoutCache,
        VulkanPipelineLayoutCache* pipelineLayoutCache,
        const VulkanPipelineStateInfo& info, 
        VkRenderPass renderPass,
        uint32_t subpass,
        const VulkanReflectedPipeline* reflection = nullptr);

    /// @brief Destructor
    ~VulkanPipeline();

    const VulkanReflectedPipeline& GetReflection() const;
    VkPipelineLayout GetPipelineLayout() const;
    VkPipeline GetPipeline() const;
    const std::map<uint32_t, VkDescriptorSetLayout>& GetDescriptorSetLayouts() const;

    void SetVertexInputState(

    );
    void SetRasterizerState(const VulkanPipelineStateInfo::RasterizerState& state);
    void SetMultisampleState(const VulkanPipelineStateInfo::MultisampleState& state);
    void SetDepthStencilState(const VulkanPipelineStateInfo::DepthStencilState& state);
    void SetColorBlendState(const VulkanPipelineStateInfo::ColorBlendState& state);

private:
    /// @brief Recreates the Vulkan pipeline object, adds the old one it to the frame destroyer.
    void Recompile();

    VulkanDevice* _device;
    VulkanReflectedPipeline _reflection;
    VkPipelineLayout _layout;
    VkPipeline _pipeline;
    std::map<uint32_t, VkDescriptorSetLayout> _descriptorSetLayouts;
};

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
