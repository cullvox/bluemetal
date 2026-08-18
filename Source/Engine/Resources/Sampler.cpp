#include "Sampler.h"
#include "Core/ClassDB.h"
#include "Engine/Engine.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/VulkanConversions.h"
#include <nlohmann/json_fwd.hpp>

namespace bl {

Sampler::Sampler()
    : Resource()
{
}

Sampler::Sampler(const Sampler& copy)
    : Resource(copy)
    , _magFilter(copy._magFilter)
    , _minFilter(copy._minFilter)
    , _mipmapMode(copy._mipmapMode)
    , _addressMode(copy._addressMode)
    , _mipLodBias(copy._mipLodBias)
    , _enableAnisotropy(copy._enableAnisotropy)
    , _maxAnisotropy(copy._maxAnisotropy)
    , _compareEnable(copy._compareEnable)
    , _compareOp(copy._compareOp)
    , _minLod(copy._minLod)
    , _maxLod(copy._maxLod)
    , _borderColor(copy._borderColor)
    , _unnormalizedCoordinates(copy._unnormalizedCoordinates)
{
    RecreateSampler();
}

void Sampler::Load()
{
    const nlohmann::json& json = GetJson();

    _magFilter = json.value("magFilter", VK_FILTER_LINEAR);
    _minFilter = json.value("minFilter", VK_FILTER_LINEAR);
    _mipmapMode = json.value("mipmapMode", VK_SAMPLER_MIPMAP_MODE_LINEAR);
    _addressMode = json.value("addressMode", VK_SAMPLER_ADDRESS_MODE_REPEAT);
    _mipLodBias = json.value("mipLodBias", 0.0f);
    _enableAnisotropy = json.value("enableAnisotropy", false);
    _maxAnisotropy = json.value("maxAnisotropy", 0.0f);
    _compareEnable = json.value("compareEnable", VK_FALSE);
    _compareOp = json.value("compareOp", VK_COMPARE_OP_NEVER);
    _minLod = json.value("minLod", 0.0f);
    _maxLod = json.value("maxLod", 0.0f);
    _borderColor = json.value("borderColor", VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);
    _unnormalizedCoordinates = json.value("unnormalizedCoordinates", VK_FALSE);

    RecreateSampler();
}

void Sampler::Release()
{
    Resource::Release();
    _sampler.reset();
}

void Sampler::RecreateSampler()
{
    _sampler.reset(new VulkanSampler(
        GraphicsSystem::Get()->GetDevice(),
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

void Sampler::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("Sampler", "Resource", &Sampler::Create);
}

}