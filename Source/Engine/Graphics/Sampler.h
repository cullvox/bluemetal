#pragma once

#include "Vulkan.h"
#include "Device.h"

namespace bl {

class StatefulResource {
public:
    bool WasUpdated() { return _wasUpdated; };
    void SetUpdated() { _wasUpdated = true; }
    void Reset() { _wasUpdated = false; };

private:
    bool _wasUpdated;
};

class Sampler : public StatefulResource {
public:
    Sampler(
        Device* device,
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
    ~Sampler();

    void SetFilters(VkFilter magFilter, VkFilter minFilter);
    void SetMipmapMode(VkSamplerMipmapMode mode);
    void SetAddressMode(VkSamplerAddressMode mode);
    void SetMipLodBias(float bias);
    void SetAnisotropy(bool enableAnisotropy, float maxAnisotropy);
    void SetCompare(bool compareEnable, VkCompareOp op);
    void SetLodMinMax(float min, float max);
    void SetBorderColor(VkBorderColor color);
    void SetUnnormalizedCoordinates(VkBool32 unnormalizedCoordinates);

    VkSampler Get();

private:
    void Update();

    Device* _device;
    VkFilter magFilter;
    VkFilter minFilter;
    VkSamplerMipmapMode mipmapMode;
    VkSamplerAddressMode addressMod;
    float mipLodBias;
    bool enableAnisotropy;
    float maxAnisotropy;
    VkBool32 compareEnable;
    VkCompareOp compareOp;
    float minLod;
    float maxLod;
    VkBorderColor borderColor;
    VkBool32 unnormalizedCoordinates;
    VkSampler _sampler;
};

} // namespace bl