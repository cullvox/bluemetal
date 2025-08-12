#include "VulkanSampler.h"
#include "Core/Hash.h"

namespace bl {

VulkanSampler::VulkanSampler(
    VulkanDevice* device,
    VkFilter magFilter,
    VkFilter minFilter,
    VkSamplerMipmapMode mipmapMode,
    VkSamplerAddressMode addressMode,
    float mipLodBias,
    bool enableAnisotropy,
    float maxAnisotropy,
    VkBool32 compareEnable,
    VkCompareOp compareOp,
    float minLod,
    float maxLod,
    VkBorderColor borderColor,
    VkBool32 unnormalizedCoordinates)
    : _device(device)
    , _sampler(VK_NULL_HANDLE)
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.pNext = nullptr;
    samplerInfo.flags = 0;
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = minFilter;
    samplerInfo.mipmapMode = mipmapMode;
    samplerInfo.addressModeU = addressMode;
    samplerInfo.addressModeV = addressMode;
    samplerInfo.addressModeW = addressMode;
    samplerInfo.mipLodBias = mipLodBias;
    samplerInfo.anisotropyEnable = enableAnisotropy;
    samplerInfo.maxAnisotropy = maxAnisotropy;
    samplerInfo.compareEnable = compareEnable;
    samplerInfo.compareOp = compareOp;
    samplerInfo.minLod = minLod;
    samplerInfo.maxLod = maxLod;
    samplerInfo.borderColor = borderColor;
    samplerInfo.unnormalizedCoordinates = unnormalizedCoordinates;
    VK_CHECK(vkCreateSampler(_device->Get(), &samplerInfo, nullptr, &_sampler));
}

VulkanSampler::~VulkanSampler()
{
    vkDestroySampler(_device->Get(), _sampler, nullptr);
}

VkSampler VulkanSampler::Get() const
{
    return _sampler;
}

} // namespace bl