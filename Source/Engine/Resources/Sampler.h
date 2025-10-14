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

public:

    Sampler();

    /**
     * @brief Constructs a new Sampler object.
     * @param manager Pointer to the resource manager.
     * @param data JSON data describing the sampler resource.
     * @param device Pointer to the Vulkan device used for creating the sampler.
     */
    Sampler(const std::filesystem::path& path);

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
};

}