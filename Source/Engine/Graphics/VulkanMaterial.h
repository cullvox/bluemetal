#pragma once

#include "Graphics/VulkanConfig.h"
#include "Math/Math.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanSampler.h"
#include "VulkanImage.h"
#include "VulkanRenderData.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanMaterialInstance.h"

namespace bl 
{

/** @brief Shader uniforms merged together in an uncomfortably useful unison.
 * 
 * A brief explanation of descriptor sets in materials.
 *
 * Materials are limited to using descriptor set index one. Therefore
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
    /// @brief Material Constructor
    /// This constructor creates a whole material object.
    ///
    /// @param device Device used to create this material.
    /// @param pass Render pass the material's pipeline is using from the render.  
    /// @param subpass The render passes subpass to use.
    /// @param state Pipeline state info.
    /// @param descriptorSetIndex Descriptor set to use for material operations, usually [1].
    VulkanMaterial(
        VulkanDevice* device,
        VkRenderPass pass,
        uint32_t subpass,
        const VulkanPipelineStateInfo& state,
        uint32_t imageCount,
        uint32_t descriptorSetIndex = 1);

    /// @brief Destructor
    ~VulkanMaterial();


    VulkanPipeline* GetPipeline() { return _pipeline.get(); }
    const std::map<std::string, VulkanVariableBlock>& GetUniforms() const { return _uniforms; }
    const std::map<std::string, uint32_t>& GetSamplers() const { return _samplers; }

    VulkanMaterialInstance* CreateInstance();

    void SetRasterizerState();

    // Allows you to change recompilable options of the pipeline.

private:
    friend class VulkanMaterialInstance;

    std::map<std::string, VulkanVariableBlock> _uniforms; 
    std::map<std::string, uint32_t> _samplers; /** @brief Name -> Binding */
    uint32_t _imageCount;
    VkDescriptorSetLayout _layout;
    VulkanDevice* _device;
    std::unique_ptr<VulkanPipeline> _pipeline;
    VulkanDescriptorSetAllocatorCache _descriptorSetCache;
};

template<typename T>
void VulkanMaterialInstance::SetGenericUniform(const std::string& name, T value) {
    const auto& uniforms = _material->GetUniforms();
    if (!uniforms.contains(name)) {
        blError("Could not set material uniform, it does not exist!");
        return;
    }

    const VulkanVariableBlock& variable = uniforms.at(name);
    assert(sizeof(T) == variable.GetSize() && "Type must be the same as the uniform size!");

    VulkanBuffer& buffer = std::get<VulkanBuffer>(_bindings[variable.GetBinding()]);
    VkDeviceSize blockSize = buffer.GetSize() / VulkanConfig::numFramesInFlight;

    auto offset = (blockSize * _currentFrame) + variable.GetOffset();

    char* uniform = nullptr;
    buffer.Map((void**)&uniform);

    // Offset the uniform to where the variable is located and copy to.
    uniform += offset;
    std::memcpy(uniform, &value, sizeof(T));

    // The uniform data has been changed, unmap and flush.
    buffer.Unmap();
    buffer.Flush(offset, sizeof(T));

    // The buffer data is marked dirty so when available the changed data 
    // will be copied to other parts of the buffer.
    SetBindingDirty(variable.GetBinding()); 
}

} // namespace bl