#pragma once

#include <vector>
#include <map>
#include <bitset>

#include "Core/Flags.h"
#include "Math/Math.h"
#include "VulkanBufferFrameRing.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanConfig.h"
#include "VulkanSampler.h"

namespace bl {


class VulkanMaterial;
class RenderData;

/**
 * @class VulkanMaterialInstance
 * @brief An instance of material data to be bound when rendering.
 *
 * A material instance contains uniform buffers and sampler descriptors for binding
 * to the renderer.
 *
 * The material instance holds a lot of the base logic for the VulkanMaterial
 * class. Materials themselves are material instances considering they hold the parent information
 * for all material instances. When a new instance is made it's parent data is copied to the new
 * instance.
 */
class VulkanMaterialInstance {
public:
    /**
     * @brief Base constructor for a material object.
     * Only used for material constructor.
     *
     * @param device The vulkan device used.
     */
    VulkanMaterialInstance(VulkanDevice* device);

    /**
     * @brief Creates a material instance.
     * @param device Vulkan device to create the material instance with.
     * @param material Vulkan material to create the instance for.
     *
     * This constructor initializes the material instance with the given device and material.
     * Only the material should call this constructor.
     */
    VulkanMaterialInstance(VulkanDevice* device, VulkanMaterial* material);

    /**
     * @brief Destroy the material instance.
     */
    ~VulkanMaterialInstance();

    /**
     * @brief Sets a boolean value in a material uniform.
     * @param name Name of the material uniform parameter to set.
     * @param value Value to set the boolean uniform.
     */
    void SetBool(const std::string& name, bool value);

    /**
     * @brief Sets an integer value in a material uniform.
     * @param name Name of the material uniform parameter to set.
     * @param value Value to set the integer.
     */
    void SetInteger(const std::string& name, int value);

    /**
     * @brief Sets a boolean value in a material uniform.
     * @param name Name of the material uniform parameter to set.
     * @param value Value to set the boolean.
     */
    void SetScaler(const std::string& name, float value);

    /**
     * @brief Sets a Vector2 value in a material uniform.
     * @param name Name of the material uniform parameter to set.
     * @param value Value to set the Vector2.
     */
    void SetVector2(const std::string& name, glm::vec2 value);

    /**
     * @brief Sets a Vector3 value in a material uniform.
     * @param name Name of the material uniform parameter to set.
     * @param value Value to set the Vector3.
     */
    void SetVector3(const std::string& name, glm::vec3 value);

    /**
     * @brief Sets a Vector4 value in a material uniform.
     * @param name Name of the material uniform parameter to set.
     * @param value Value to set the Vector4.
     */
    void SetVector4(const std::string& name, glm::vec4 value);

    /**
     * @brief Sets a Matrix value in a material uniform.
     * @param name Name of the material uniform parameter to set.
     * @param value Value to set the Matrix.
     */
    void SetMatrix(const std::string& name, glm::mat4 value);

    /**
     * @brief Sets a sampled image with a sampler.
     * @param name Name of the material uniform parameter to set.
     * @param sampler Sampler to use for the image.
     * @param image Image to set as a sampled image.
     */
    void SetSampledImage2D(const std::string& name, VulkanSampler* sampler, VulkanImage* image);

    /**
     * @brief Updates and cleans descriptor set data for this frame.
     * @details This function must be called before the renderer starts rendering the frame.
     * It will update the descriptor sets for the current frame and ensure that all bindings are up
     * to date.
     */
    void UpdateUniforms(uint32_t currentImage);

    /**
     * @brief Binds the material instance to the current render data.
     * @param rd Render data to bind the material instance to.
     */
    void Bind(RenderData& rd) const;

    /**
     * @brief Pushes a constant value to the current command buffer.
     * @param rd Render data to push the constant to.
     * @param offset Offset in bytes to push the constant.
     * @param size Size in bytes of the constant.
     * @param value Pointer to the constant data.
     */
    void PushConstant(RenderData& rd, uint32_t offset, uint32_t size, const void* value) const;

    /**
     * @brief Pushes a constant value to the current command buffer.
     * @param rd Render data to push the constant to.
     * @param offset Offset in bytes to push the constant.
     * @param data Constant data to push.
     */
    template <typename T>
    void PushConstant(RenderData& rd, uint32_t offset, const T& data) const
    {
        PushConstant(rd, offset, sizeof(T), &data);
    }

    /**
     * @brief Returns the base material.
     */
    VulkanMaterial* GetBaseMaterial();

    template<typename T>
    void GetGenericUniform(const std::string& name, T& value);

protected:
    /**
     * @brief Creates the per frame descriptor sets for the material instance.
     * @param layout Descriptor set layout to create the per frame bindings for.
     */
    void BuildPerFrameBindings(VkDescriptorSetLayout layout);

    /**
     * @brief Sets a binding dirty for all frames except the current frame.
     * @param binding Binding to set dirty.
     */
    void SetBindingDirty(uint32_t binding);

    /**
     * @brief Sets a generic uniform value in the material instance.
     * @param name Name of the material uniform parameter to set.
     * @param value Value to set the uniform.
     *
     * This function is used to set a uniform value in the material instance.
     * It will check if the uniform exists and if it does, it will set the value.
     */
    template <typename T>
    void SetGenericUniform(const std::string& name, T value);



private:
    friend class VulkanMaterial;

    /**
     * @struct PerFrameData
     * @brief Data for each frame that needs to be updated.
     *
     * Descriptor sets cannot be changed while bounded in a command buffer.
     * Effectively making them useless while rendering. To solve this we use
     * multiple descriptor sets for a material to ensure that we are never using
     * a descriptor set while it's bound.
     */
    struct PerFrameData {
        std::unique_ptr<VulkanDescriptorSet> set;
        std::unordered_map<int, VulkanBuffer> buffer;
        std::bitset<32> dirty; /// @brief If a binding is dirty it must be updated somehow.
    };

    /**
     * @struct SampledImage
     */
    struct SampledImageData {
        VulkanSampler* sampler;
        VulkanImage* image;
    };

    struct UniformData {
        std::vector<std::byte> data;
        VulkanBufferFrameRing buffer;
    };

    using BindingData = std::variant<UniformData, SampledImageData>;

    VulkanDevice* _device;
    VulkanMaterial* _material;
    int32_t _materialSet;
    uint32_t _currentFrame;
    std::map<uint32_t, BindingData> _bindings;
    std::array<PerFrameData, VulkanConfig::maxFramesInFlight> _perFrameData;
    std::array<VkDescriptorSet, 4> _sets;

    void FreeSets(); /// Frees all descriptor sets back into the descriptor set pool.

};

}