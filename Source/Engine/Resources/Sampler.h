#pragma once

#include "Core/Object.h"
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
    OBJECT_BOILER(Sampler, Resource)

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
    Sampler(const Sampler& copy);
    virtual ~Sampler() = default;

    static void RegisterClass();

    virtual void Load() override;
    virtual void Release() override;

    Sampler& operator=(const Sampler& copy);

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