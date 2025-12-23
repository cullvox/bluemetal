#pragma once

#include "VulkanForward.h"
#include "VulkanDevice.h"

namespace bl {

/**
 * @class VulkanSampler
 * @brief A Vulkan sampler object for texture sampling.
 */
class VulkanSampler {
public:
    /**
     * @brief Constructs a Vulkan sampler with specified parameters.
     * @param device Vulkan device to create the sampler with.
     * @param magFilter Magnification filter to use.
     * @param minFilter Minification filter to use.
     * @param mipmapMode Mipmap mode to use.
     * @param addressMode Address mode to use for texture coordinates.
     * @param mipLodBias Mipmap level of detail bias.
     * @param enableAnisotropy Enable anisotropic filtering.
     * @param maxAnisotropy Maximum anisotropy level.
     * @param compareEnable Enable comparison for the sampler.
     * @param compareOp Comparison operation to use.
     * @param minLod Minimum level of detail for the sampler.
     * @param maxLod Maximum level of detail for the sampler.
     * @param borderColor Border color to use for the sampler.
     * @param unnormalizedCoordinates Use unnormalized coordinates for the sampler.
     */
    VulkanSampler(
        VulkanDevice* device,
        VkFilter magFilter = VK_FILTER_LINEAR,
        VkFilter minFilter = VK_FILTER_LINEAR,
        VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        float mipLodBias = 0.0f,
        bool enableAnisotropy = VK_FALSE,
        float maxAnisotropy = 0.0f,
        VkBool32 compareEnable = VK_FALSE,
        VkCompareOp compareOp = VK_COMPARE_OP_NEVER,
        float minLod = 0.0f,
        float maxLod = 0.0f,
        VkBorderColor borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        VkBool32 unnormalizedCoordinates = VK_FALSE);

    /**
     * @brief Destroys the Vulkan sampler.
     */
    ~VulkanSampler();

    /**
     * @brief Returns the Vulkan sampler handle.
     * @return The Vulkan sampler handle.
     */
    VkSampler Get() const;

private:
    VulkanDevice* _device;
    VkSampler _sampler;
};

} // namespace bl