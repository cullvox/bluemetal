#pragma once

#include "Graphics/VulkanSampler.h"
#include "Resource.h"

namespace bl {

class VulkanDevice;
class GraphicsSystem;
class ResourceSystem;

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
    Sampler(Engine& engine, const std::filesystem::path& path);
    virtual ~Sampler() = default;

    VkSampler Get() const;
    VulkanSampler* GetSampler() const;
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