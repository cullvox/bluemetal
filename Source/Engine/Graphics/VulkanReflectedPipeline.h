#pragma once

#include <map>
#include <vector>

#include "VulkanReflectedDescriptorSet.h"
#include "VulkanReflectedPushConstant.h"
#include "VulkanPipelineState.h"

namespace bl {

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

    uint32_t GetPipelineRequiredSetCount() const; 
};

} // namespace bl
