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

namespace bl {

/// @brief Creates a pipeline and interprets reflection data 
class MaterialPipeline : public VulkanPipeline {
public:
    MaterialPipeline(
        VulkanDevice* device, 
        VkRenderPass pass, 
        uint32_t subpass, 
        const VulkanPipelineStateInfo& state, 
        uint32_t materialSet = 1);

    ~MaterialPipeline();

    /// @brief Returns the device that this pipeline was created with.
    /// Makes it easier if we are friends with an instance. 
    const std::map<std::string, VulkanVariableBlock>& GetUniforms() const { return _uniforms; }
    const std::map<std::string, uint32_t>& GetSamplers() const { return _samplers; }
    VkDescriptorSetLayout GetDescriptorSetLayout() const { return _layout; }

private:
    std::map<std::string, VulkanVariableBlock> _uniforms; 
    std::map<std::string, uint32_t> _samplers; /** @brief Name -> Binding */
    VkDescriptorSetLayout _layout;
};

class Material;

/// @brief An independent material data for sampled images and uniform buffer bindings.
///
///
class MaterialInstance {
public:
    ~MaterialInstance();

    void SetBool(const std::string& name, bool value);
    void SetInteger(const std::string& name, int value);
    void SetScaler(const std::string& name, float value);
    void SetVector2(const std::string& name, glm::vec2 value);
    void SetVector3(const std::string& name, glm::vec3 value);
    void SetVector4(const std::string& name, glm::vec4 value);
    void SetMatrix(const std::string& name, glm::mat4 value);
    void SetSampledImage2D(const std::string& name, VulkanSampler* sampler, VulkanImage* image);
    void UpdateUniforms(); /** @brief This function must be called before the renderer starts rendering the frame. */

    void Bind(VulkanRenderData& rd); /** @brief Bind this material for rending using it and it's data. */
    void PushConstant(VulkanRenderData& rd, uint32_t offset, uint32_t size, const void* value);

protected:
    /// @brief Creates an independent instance of material data and copies from parent.
    ///
    ///
    MaterialInstance(
        VulkanDevice* device,
        Material* material);

protected:
    void BuildPerFrameBindings(VkDescriptorSetLayout layout);
    void SetBindingDirty(uint32_t binding);
    size_t CalculateDynamicAlignment(size_t uboSize);

    template<typename T> 
    void SetGenericUniform(const std::string& name, T value);

private:
    /// @brief Material data that needs to be changed every frame.
    ///
    /// Descriptor sets cannot be changed while bounded in a command buffer. 
    /// Effectively making them useless while rendering. To solve this we use 
    /// multiple descriptor sets for a material to ensure that we are never using 
    /// a descriptor set while it's bound. 
    struct PerFrameData {
        VkDescriptorSet set;
        std::map<uint32_t, bool> dirty; /// @brief If a binding is dirty it must be updated somehow.
    };

    /// @brief Combined sampler and image.
    struct SampledImage {
        VulkanMutableReference<VulkanSampler> sampler;
        VulkanImage* image;
    };

    using BindingData = std::variant<VulkanBuffer, SampledImage>;

    VulkanDevice* _device;
    Material* _material;
    uint32_t _materialSet;
    uint32_t _currentFrame;
    std::map<uint32_t, BindingData> _bindings;
    std::array<PerFrameData, VulkanConfig::numFramesInFlight> _perFrameData;
};

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
class Material : public MaterialPipeline, public MaterialInstance {
public:
    /// @brief Material Constructor
    /// This constructor creates a whole material object.
    ///
    /// @param device Device used to create this material.
    /// @param pass Render pass the material's pipeline is using from the render.  
    /// @param subpass The render passes subpass to use.
    /// @param state Pipeline state info.
    /// @param descriptorSetIndex Descriptor set to use for material operations, usually [1].
    Material(
        VulkanDevice* device,
        VkRenderPass pass,
        uint32_t subpass,
        const VulkanPipelineStateInfo& state,
        uint32_t descriptorSetIndex = 1);

    /// @brief Destructor
    ~Material();

    MaterialInstance* CreateInstance();

    void SetRasterizerState();

    // Allows you to change recompilable options of the pipeline.

protected:
    friend class MaterialInstance;
    VulkanDescriptorSetAllocatorCache* GetDescriptorSetCache();

private:
    VulkanDevice* _device;
    VulkanDescriptorSetAllocatorCache _descriptorSetCache;
};

template<typename T>
void MaterialInstance::SetGenericUniform(const std::string& name, T value) {
    const auto& uniforms = _pipeline->GetUniforms();
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