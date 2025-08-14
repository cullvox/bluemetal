#pragma once

#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanSampler.h"
#include "Resource.h"

namespace bl {

/**
 * @class Sampler
 * @brief A texture sampler resource.
 */
class Sampler : public Resource {
    VulkanDevice* _device;
    VkFilter _magFilter;
    VkFilter _minFilter;
    VkSamplerMipmapMode _mipmapMode;
    VkSamplerAddressMode _addressMode;
    float _mipLodBias;
    bool _enableAnisotropy;
    float _maxAnisotropy;
    VkBool32 _compareEnable;
    VkCompareOp _compareOp;
    float _minLod;
    float _maxLod;
    VkBorderColor _borderColor;
    VkBool32 _unnormalizedCoordinates;
    std::unique_ptr<VulkanSampler> _sampler;

    void RecreateSampler();

protected:
    virtual bool Load() override;
    virtual void Unload() override;
    virtual bool ExportBinary(std::ostream& stream) const override;

public:

    /**
     * @brief Constructs a new Sampler object.
     * @param manager Pointer to the resource manager.
     * @param data JSON data describing the sampler resource.
     * @param device Pointer to the Vulkan device used for creating the sampler.
     */
    Sampler();

    /**
     * @brief Destructor for the Sampler class.
     */
    virtual ~Sampler() = default;

    VkSampler Get() const;
    VkFilter GetMagFilter() const;
    VkFilter GetMinFilter() const;
    VkSamplerMipmapMode GetMipmapMode() const;
    VkSamplerAddressMode GetAddressMode() const;
    float GetMipLodBias() const;
    bool IsAnisotropyEnabled() const;
    float GetMaxAnisotropy() const;
    VkBool32 IsCompareEnabled() const;
    VkCompareOp GetCompareOp() const;
    float GetMinLod() const;
    float GetMaxLod() const;
    VkBorderColor GetBorderColor() const;
    VkBool32 IsUnnormalizedCoordinates() const;

    void Set(
        VkFilter magFilter = VK_FILTER_LINEAR,
        VkFilter minFilter = VK_FILTER_LINEAR,
        VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        float mipLodBias = 0.0f,
        bool enableAnisotropy = VK_FALSE,
        float maxAnisotropy = 0.0f,
        VkBool32 compareEnable = VK_FALSE,
        VkCompareOp compareOp = VK_COMPARE_OP_NEVER,
        float minLod = 0.0f,
        float maxLod = 0.0f,
        VkBorderColor borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        VkBool32 unnormalizedCoordinates = VK_FALSE);
};

}