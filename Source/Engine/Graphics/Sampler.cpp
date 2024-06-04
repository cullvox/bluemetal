#include "Sampler.h"

Sampler::Sampler(
    Device* device,
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
    , _magFilter(magFilter)
    , _minFilter(minFilter)
    , _mipmapMode(mipmapMode)
    , _addressMode(addressMode)
    , _mipLodBias(mipLodBias)
    , _anisotropyEnable(anisotropyEnable)
    , _maxAnisotropy(maxAnisotropy)
    , _compareEnable(compareEnable)
    , _compareOp(compareOp)
    , _minLod(minLod)
    , _maxLod(maxLod)
    , _borderColor(borderColor)
    , _unnormalizedCoordinates(unnormalizedCoordinates)
    , _sampler(VK_NULL_HANDLE) {
    Update();
}

Sampler::Update()
{
    _device->PushPostFrameDeleter([&](){
        vkDestroySampler(_device->Get(), _sampler, nullptr);
    });

    VkSamplerCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.magFilter = _magFilter;
    createInfo.minFilter = _minFilter;
    createInfo.mipmapMode = _mipmapMode,
    createInfo.addressModeU = _addressMode;
    createInfo.addressModeV = _addressMode;
    createInfo.addressModeW = _addressMode;
    createInfo.mipLodBias = _mipLodBias;
    createInfo.anisotropyEnable = _anisotropyEnable;
    createInfo.maxAnisotropy = _maxAnisotropy;
    createInfo.compareEnable = _compareEnable;
    createInfo.compareOp = _compareOp;
    createInfo.minLod = _minLod;
    createInfo.maxLod = _maxLod;
    createInfo.borderColor = _borderColor;
    createInfo.unnormalizedCoordinates = _unnormalizedCoordinates;

    VK_CHECK(vkCreateSampler(_device->Get(), &createInfo, nullptr, &_sampler));
    SetUpdated();
}

Sampler::~Sampler()
{
    _device->PushFrameDeleter([&](){
        vkDestroySampler(_device->Get(), _sampler, nullptr);
    });
}
