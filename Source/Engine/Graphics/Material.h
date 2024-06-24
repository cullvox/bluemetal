#pragma once

#include "Graphics/VulkanConfig.h"
#include "Math/Math.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanSampler.h"
#include "VulkanImage.h"
#include "VulkanRenderData.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorSetCache.h"

namespace bl {

class MaterialInstance;


/// @brief Basic data used in a material but not copied into instances.
class MaterialPipeline {
public:
    std::map<std::string, VulkanBlockVariable> _uniforms; 
    std::map<std::string, uint32_t> _samplers; /** @brief Name -> Binding */
    VkDescriptorSetLayout _layout;
    VulkanPipeline _pipeline;
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
class Material
{
public:

    /// @brief Constructor
    /// This constructor creates a basic copy of per frame descriptor set data.
    /// Only use this for material instancing. 
    ///
    Material();

    /// @brief Material Constructor
    /// This constructor creates a whole material object.
    ///
    /// @param device Device used to create this material.
    /// @param pass Render pass the material's pipeline is using from the render.  
    /// @param subpass The render passes subpass to use.
    /// @param state Pipeline state info.
    /// @param materialSet Descriptor set to use for material operations.
    Material(VulkanDevice* device, VkRenderPass pass, uint32_t subpass, const VulkanPipelineStateInfo& state, uint32_t materialSet = 1);

    /// @brief Destructor
    ~Material();

    void SetBoolean(const std::string& name, bool value);
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
        VulkanMutableResourceReference<VulkanSampler> sampler;
        VulkanImage* image;
    };

    using BindingData = std::variant<VulkanBuffer, SampledImage>;

    void BuildPerFrameBindings(VkDescriptorSetLayout layout);
    void SetBindingDirty(uint32_t binding);
    
    size_t CalculateDynamicAlignment(size_t uboSize);
    template<typename T> 
    void SetGenericUniform(const std::string& name, T value);

    VulkanDevice* _device;
    std::unique_ptr<BaseData> _data;
    uint32_t _materialSet;
    uint32_t _currentFrame;
    std::map<uint32_t, BindingData> _data;
    std::array<PerFrameData, VulkanConfig::numFramesInFlight> _perFrameData;
};

class MaterialInstance : public Material {
public:
    MaterialInstance(Material* base);
    ~MaterialInstance();

    Material* GetBaseMaterial();

private:
    Material* _base;
};

template<typename T>
void Material::SetGenericUniform(const std::string& name, T value)
{
    if (!GetUniformMetadata().contains(name))
    {
        blError("Could not set material uniform as it does not exist!");
        return;
    }

    auto meta = GetUniformMetadata().at(name);
    assert(sizeof(T) == meta.GetSize() && "Type must be the same as the uniform size!");

    auto& buffer = std::get<VulkanBuffer>(_data[meta.GetBinding()]);
    VkDeviceSize blockSize = buffer.GetSize() / VulkanConfig::numFramesInFlight;
    // uint32_t previousFrame = (_currentFrame - 1) % GraphicsConfig::numFramesInFlight;

    auto offset = (blockSize * _currentFrame) + meta.GetOffset();

    char* uniform = nullptr;
    buffer.Map((void**)&uniform);
    uniform += offset;
    std::memcpy(uniform, &value, sizeof(T));
    buffer.Unmap();

    buffer.Flush(offset, sizeof(T));

    SetBindingDirty(meta.GetBinding()); /* The buffer data must be copied to the other parts of the dynamic uniform buffer. */
}

} // namespace bl