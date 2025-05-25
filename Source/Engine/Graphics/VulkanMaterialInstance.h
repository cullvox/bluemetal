#pragma once

#include "Math/Math.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanSampler.h"
#include "VulkanImage.h"
#include "VulkanRenderData.h"

namespace bl
{

class VulkanMaterial;

/// @brief An independent material data for sampled images and uniform buffer bindings.
///
///
class VulkanMaterialInstance 
{
public:
    ~VulkanMaterialInstance();

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
    VulkanMaterialInstance(VulkanDevice* device, VulkanMaterial* material);

protected:
    void BuildPerFrameBindings(VkDescriptorSetLayout layout);
    void SetBindingDirty(uint32_t binding);
    size_t CalculateDynamicAlignment(size_t uboSize);

    template<typename T> 
    void SetGenericUniform(const std::string& name, T value);

private:
    friend class VulkanMaterial;
    /// @brief Material data that needs to be changed every frame.
    ///
    /// Descriptor sets cannot be changed while bounded in a command buffer. 
    /// Effectively making them useless while rendering. To solve this we use 
    /// multiple descriptor sets for a material to ensure that we are never using 
    /// a descriptor set while it's bound. 
    struct PerFrameData 
    {
        VkDescriptorSet set;
        std::map<uint32_t, bool> dirty; /// @brief If a binding is dirty it must be updated somehow.
    };

    /// @brief Combined sampler and image.
    struct SampledImage 
    {
        VulkanSampler* sampler;
        VulkanImage* image;
    };

    using BindingData = std::variant<VulkanBuffer, SampledImage>;

    VulkanDevice* _device;
    VulkanMaterial* _material;
    uint32_t _materialSet;
    uint32_t _currentFrame;
    std::map<uint32_t, BindingData> _bindings;
    std::vector<PerFrameData> _perFrameData;
};

}