#pragma once

#include <map>

#include <nlohmann/json.hpp>

#include "Vulkan.h"
#include "VulkanConversions.h"
#include "VulkanDescriptorSetLayoutCache.h"
#include "VulkanPipelineState.h"
#include "VulkanReflectedPipeline.h"

namespace bl {

class VulkanDevice;
class VulkanShader;

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
