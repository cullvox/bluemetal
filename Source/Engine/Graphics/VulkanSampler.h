#pragma once

#include "Vulkan.h"
#include "VulkanDevice.h"

namespace bl {

class VulkanSampler {
public:
    VulkanSampler(
        VulkanDevice* device,
        VkFilter magFilter = VK_FILTER_LINEAR,
        VkFilter minFilter = VK_FILTER_LINEAR,
        VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        float mipLodBias = 0.0f,
        bool enableAnisotropy = VK_FALSE,
        float maxAnisotropy = 0.0f,
        VkBool32 compareEnable = VK_FALSE,
        VkCompareOp compareOp = VK_COMPARE_OP_NEVER,
        float minLod = 0.0f,
        float maxLod = 0.0f,
        VkBorderColor borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        VkBool32 unnormalizedCoordinates = VK_FALSE);
    ~VulkanSampler();

    VkSampler Get() const;
    void SetFilters(VkFilter magFilter, VkFilter minFilter);
    void SetMipmapMode(VkSamplerMipmapMode mode);
    void SetAddressMode(VkSamplerAddressMode mode);
    void SetMipLodBias(float bias);
    void SetAnisotropy(bool enableAnisotropy, float maxAnisotropy);
    void SetCompare(bool compareEnable, VkCompareOp op);
    void SetLodMinMax(float min, float max);
    void SetBorderColor(VkBorderColor color);
    void SetUnnormalizedCoordinates(VkBool32 unnormalizedCoordinates);


private:
    void Update();

    VulkanDevice* _device;
    VkSampler _sampler;
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
};

} // namespace bl