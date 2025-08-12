#include "Sampler.h"

namespace bl {

Sampler::Sampler(ResourceManager* manager, VulkanDevice* device)
    : Resource(manager)
    , _device(device)
    , _magFilter(VK_FILTER_LINEAR)
    , _minFilter(VK_FILTER_LINEAR)
    , _mipmapMode(VK_SAMPLER_MIPMAP_MODE_LINEAR)
    , _addressMode(VK_SAMPLER_ADDRESS_MODE_REPEAT)
    , _mipLodBias(0.0f)
    , _enableAnisotropy(false)
    , _maxAnisotropy(0.0f)
    , _compareEnable(VK_FALSE)
    , _compareOp(VK_COMPARE_OP_NEVER)
    , _minLod(0.0f)
    , _maxLod(0.0f)
    , _borderColor(VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK)
    , _unnormalizedCoordinates(VK_FALSE)
{
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
    UpdateReferences();
}

bool Sampler::Load()
{
    if (IsLoaded())
        return true;

    nlohmann::json data;

    switch (GetSource()) {
    case ResourceSource::eFile: {
        std::ifstream file(GetPath());
        if (!file.is_open()) {
            Log::Error("Failed to open sampler file: {}", GetPath());
            return false;
        }
        file >> data;
        file.close();
        break;
    }
    case ResourceSource::eBinary: {
        auto binaryData = GetBinaryStream();
        if (binaryData.empty()) {
            Log::Error("Binary data for sampler is empty.");
            return false;
        }
        data = nlohmann::json::from_cbor(binaryData); // Assuming binary data is in CBOR format
        break;
    }
    }

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
    isLoaded = true;
}

void Sampler::Unload()
{
    if (isLoaded) {
        _sampler.reset();
        isLoaded = false;
    }
}

bool Sampler::ExportBinary(std::ostream& stream) const
{
    nlohmann::json data;
    data["magFilter"] = GetMagFilter();
    data["minFilter"] = GetMinFilter();
    data["mipmapMode"] = GetMipmapMode();
    data["addressMode"] = GetAddressMode();
    data["mipLodBias"] = GetMipLodBias();
    data["enableAnisotropy"] = IsAnisotropyEnabled();
    data["maxAnisotropy"] = GetMaxAnisotropy();
    data["compareEnable"] = IsCompareEnabled();
    data["compareOp"] = GetCompareOp();
    data["minLod"] = GetMinLod();
    data["maxLod"] = GetMaxLod();
    data["borderColor"] = GetBorderColor();
    data["unnormalizedCoordinates"] = IsUnnormalizedCoordinates();

    auto cbor = nlohmann::json::to_cbor(data); // Save as binary
    std::copy(cbor.begin(), cbor.end(), std::ostream_iterator<char>(stream));
    return stream.good();
}

void Sampler::Set(
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
{
    _magFilter = magFilter;
    _minFilter = minFilter;
    _mipmapMode = mipmapMode;
    _addressMode = addressMode;
    _mipLodBias = mipLodBias;
    _enableAnisotropy = enableAnisotropy;
    _maxAnisotropy = maxAnisotropy;
    _compareEnable = compareEnable;
    _compareOp = compareOp;
    _minLod = minLod;
    _maxLod = maxLod;
    _borderColor = borderColor;
    _unnormalizedCoordinates = unnormalizedCoordinates;
    RecreateSampler();
}

VkSampler Sampler::Get() const
{
    return _sampler ? _sampler->Get() : VK_NULL_HANDLE;
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