#include "Sampler.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/VulkanConversions.h"

namespace bl {

Sampler::Sampler(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path)
    : Resource(resourceSystem, system, path)
    , _device(system->GetDevice())
{
    nlohmann::json data;
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open sampler JSON file.");
    }
    file >> data;
    file.close();

    _magFilter = data.value("magFilter", VK_FILTER_LINEAR);
    _minFilter = data.value("minFilter", VK_FILTER_LINEAR);
    _mipmapMode = data.value("mipmapMode", VK_SAMPLER_MIPMAP_MODE_LINEAR);
    _addressMode = data.value("addressMode", VK_SAMPLER_ADDRESS_MODE_REPEAT);
    _mipLodBias = data.value("mipLodBias", 0.0f);
    _enableAnisotropy = data.value("enableAnisotropy", false);
    _maxAnisotropy = data.value("maxAnisotropy", 0.0f);
    _compareEnable = data.value("compareEnable", VK_FALSE);
    _compareOp = data.value("compareOp", VK_COMPARE_OP_NEVER);
    _minLod = data.value("minLod", 0.0f);
    _maxLod = data.value("maxLod", 0.0f);
    _borderColor = data.value("borderColor", VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);
    _unnormalizedCoordinates = data.value("unnormalizedCoordinates", VK_FALSE);

    RecreateSampler();
}

void Sampler::RecreateSampler()
{
    _sampler.reset(new VulkanSampler(
        _device,
        GetMagFilter(),
        GetMinFilter(),
        GetMipmapMode(),
        GetAddressMode(),
        GetMipLodBias(),
        IsAnisotropyEnabled(),
        GetMaxAnisotropy(),
        IsCompareEnabled(),
        GetCompareOp(),
        GetMinLod(),
        GetMaxLod(),
        GetBorderColor(),
        IsUnnormalizedCoordinates()));
}

VkSampler Sampler::Get() const
{
    return _sampler.get() ? _sampler.get()->Get() : VK_NULL_HANDLE;
}

VulkanSampler* Sampler::GetSampler() const
{
    return _sampler.get();
}

VkFilter Sampler::GetMagFilter() const
{
    return _magFilter;
}

VkFilter Sampler::GetMinFilter() const
{
    return _minFilter;
}

VkSamplerMipmapMode Sampler::GetMipmapMode() const
{
    return _mipmapMode;
}

VkSamplerAddressMode Sampler::GetAddressMode() const
{
    return _addressMode;
}

float Sampler::GetMipLodBias() const
{
    return _mipLodBias;
}

bool Sampler::IsAnisotropyEnabled() const
{
    return _enableAnisotropy;
}

float Sampler::GetMaxAnisotropy() const
{
    return _maxAnisotropy;
}

VkBool32 Sampler::IsCompareEnabled() const
{
    return _compareEnable;
}

VkCompareOp Sampler::GetCompareOp() const
{
    return _compareOp;
}

float Sampler::GetMinLod() const
{
    return _minLod;
}

float Sampler::GetMaxLod() const
{
    return _maxLod;
}

VkBorderColor Sampler::GetBorderColor() const
{
    return _borderColor;
}

VkBool32 Sampler::IsUnnormalizedCoordinates() const
{
    return _unnormalizedCoordinates;
}

}