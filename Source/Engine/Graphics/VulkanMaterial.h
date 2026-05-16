#pragma once

#include "Core/Print.h"
#include "VulkanBuffer.h"
#include "VulkanConfig.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanDevice.h"
#include "VulkanMaterialInstance.h"
#include "VulkanPipeline.h"

namespace bl {


enum class VulkanMaterialSupportFlags : uint32_t
{
    eNone           = 0x00000000,
    eGlobalBuffer   = 0x00000001,
    eInstanceBuffer = 0x00000002,
    eMaterialBuffer = 0x00000004,
};

inline constexpr VulkanMaterialSupportFlags operator|(VulkanMaterialSupportFlags lhs, VulkanMaterialSupportFlags rhs) noexcept {
    return static_cast<VulkanMaterialSupportFlags>(
        static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs)
    );
}

inline constexpr VulkanMaterialSupportFlags operator&(VulkanMaterialSupportFlags lhs, VulkanMaterialSupportFlags rhs) noexcept {
    return static_cast<VulkanMaterialSupportFlags>(
        static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs)
    );
}

inline VulkanMaterialSupportFlags& operator|=(VulkanMaterialSupportFlags& lhs, VulkanMaterialSupportFlags rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

inline VulkanMaterialSupportFlags& operator&=(VulkanMaterialSupportFlags& lhs, VulkanMaterialSupportFlags rhs) noexcept {
    lhs = lhs & rhs;
    return lhs;
}

class Renderer;

/**
 * @class VulkanMaterial
 *
 * @brief A pipeline with updatable uniform buffers and texture samplers.
 *
 * Materials allow the user to change a shaders uniform buffers and their properties.
 * While access is still relatively low level in this class
 * Materials are limited to using only one descriptor set index. Therefore
 * bindings and all material data are all inside of one set and limited.
 * Many material instances can be created from one material allowing
 * variation with material uniforms in a simple and effectively understandable
 * way.
 *
 * The renderer is supposed to handle descriptor sets zero and two for
 * global and instance rendering data respectively.
 *
 */
class VulkanMaterial : public VulkanMaterialInstance {

public:
    /**
     * @brief Map from uniform name to a uniform variable block.
     */
    using UniformMap = std::map<std::string, VulkanVariableBlock>;

    /**
     * @brief Map from sampler name to a sampler binding.
     */
    using SamplerMap = std::map<std::string, uint32_t>;

    /** @brief Material Constructor
     * This constructor creates a whole material object.
     *
     * @param device Device used to create this material.
     * @param pass Render pass the material's pipeline is using from the render.
     * @param subpass The render passes subpass to use.
     * @param state Pipeline state info.
     * @param descriptorSetIndex Descriptor set to use for material operations, usually [1].
     */
    VulkanMaterial(
        VulkanDevice* device,
        Renderer* renderer,
        const VulkanPipelineStateInfo& state,
        int32_t descriptorSetIndex = 2);

    /**
     * @brief Destroys the Vulkan material.
     * @details This will destroy the pipeline and all resources associated with it.
     */
    ~VulkanMaterial();

    /**
     * @brief Gets the pipeline used by this material.
     * @return The Vulkan pipeline used by this material.
     */
    const VulkanPipeline* GetPipeline() const { return _pipeline.get(); }

    /**
     * @brief Gets the reflected descriptor uniforms of this material.
     * @return The reflected descriptor uniforms of this material
     *
     * This is a map of uniform names to their variable blocks.
     */
    const UniformMap& GetUniforms() const { return _uniforms; }

    /**
     * @brief Gets the reflected descriptor samplers of this material.
     * @return The reflected descriptor samplers of this material.
     *
     * This is a map of sampler names to their binding locations.
     */
    const SamplerMap& GetSamplers() const { return _samplers; }

    VulkanMaterialSupportFlags GetSupportFlags() const;

    /**
     * @brief Creates a material instance for this material.
     * @return A new VulkanMaterialInstance for this material.
     */
    std::unique_ptr<VulkanMaterialInstance> CreateInstance();

private:
    friend class VulkanMaterialInstance;

    std::unique_ptr<VulkanPipeline> _pipeline;
    UniformMap _uniforms;
    SamplerMap _samplers;
    uint32_t _swapchainImageCount;
    VkDescriptorSetLayout _layout;
    VulkanDescriptorSetAllocatorCache _descriptorSetCache;
    VulkanMaterialSupportFlags _flags;
    VkDescriptorSet _emptySet;
    uint32_t _setCount;
};

// Must be defined here because we use the material definition.
template <typename T>
void VulkanMaterialInstance::SetGenericUniform(const std::string& name, T value)
{
    const auto& uniforms = _material->GetUniforms();
    if (!uniforms.contains(name)) {
        Print::Error("Could not set material uniform, it does not exist!");
        return;
    }

    const VulkanVariableBlock& variable = uniforms.at(name);
    assert(sizeof(T) == variable.GetSize() && "Type must be the same as the uniform size!");

    UniformData& uniform = std::get<UniformData>(_bindings[variable.GetBinding()]);

    auto offset = variable.GetOffset();

    // Offset the uniform to where the variable is located and copy to.
    std::memcpy(uniform.data.data() + offset, &value, sizeof(T));

    // The buffer data is marked dirty so when available the changed data
    // will be copied to other parts of the buffer.
    SetBindingDirty(variable.GetBinding());
}

} // namespace bl