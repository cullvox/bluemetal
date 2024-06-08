#include "Sampler.h"

namespace bl {

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
    , _sampler(VK_NULL_HANDLE)
    , _magFilter(magFilter)
    , _minFilter(minFilter)
    , _mipmapMode(mipmapMode)
    , _addressMode(addressMode)
    , _mipLodBias(mipLodBias)
    , _enableAnisotropy(enableAnisotropy)
    , _maxAnisotropy(maxAnisotropy)
    , _compareEnable(compareEnable)
    , _compareOp(compareOp)
    , _minLod(minLod)
    , _maxLod(maxLod)
    , _borderColor(borderColor)
    , _unnormalizedCoordinates(unnormalizedCoordinates) {
    Update();
}

void Sampler::Update() {

    // _device->PushPostFrameDeleter([&](){
    vkDestroySampler(_device->Get(), _sampler, nullptr);
    // });

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
    createInfo.anisotropyEnable = _enableAnisotropy;
    createInfo.maxAnisotropy = _maxAnisotropy;
    createInfo.compareEnable = _compareEnable;
    createInfo.compareOp = _compareOp;
    createInfo.minLod = _minLod;
    createInfo.maxLod = _maxLod;
    createInfo.borderColor = _borderColor;
    createInfo.unnormalizedCoordinates = _unnormalizedCoordinates;

    VK_CHECK(vkCreateSampler(_device->Get(), &createInfo, nullptr, &_sampler));
}

Sampler::~Sampler() {
    // _device->PushFrameDeleter([&](){
    vkDestroySampler(_device->Get(), _sampler, nullptr);
    // });
}

VkSampler Sampler::Get() const {
    return _sampler;
}

void Sampler::SetFilters(VkFilter magFilter, VkFilter minFilter) {
    _magFilter = magFilter;
    _minFilter = minFilter;
}

void Sampler::SetMipmapMode(VkSamplerMipmapMode mode) {
    _mipmapMode = mode;
}

void Sampler::SetAddressMode(VkSamplerAddressMode mode) {
    _addressMode = mode;
}

void Sampler::SetMipLodBias(float bias) {
    _mipLodBias = bias;
}

void Sampler::SetAnisotropy(bool enableAnisotropy, float maxAnisotropy) {
    _enableAnisotropy = enableAnisotropy;
    _maxAnisotropy = maxAnisotropy;
}

void Sampler::SetCompare(bool compareEnable, VkCompareOp op) {
    _compareEnable = compareEnable;
    _compareOp = op;
}

void Sampler::SetLodMinMax(float min, float max) {
    _minLod = min;
    _maxLod = max;
}

void Sampler::SetBorderColor(VkBorderColor color) {
    _borderColor = color;
}

void Sampler::SetUnnormalizedCoordinates(VkBool32 unnormalizedCoordinates) {
    _unnormalizedCoordinates = unnormalizedCoordinates;
}

} // namespace bl