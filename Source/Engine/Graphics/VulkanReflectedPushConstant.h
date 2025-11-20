#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "VulkanReflectedBlock.h"

namespace bl {

/**
 * @class VulkanReflectedPushConstant
 * @brief Reflection data for a Vulkan push constant range.
 *
 * This class holds the reflection data for a push constant range in a Vulkan pipeline.
 * It includes the stages it is used in, the offset, and the size of the push constant.
 */
class VulkanReflectedPushConstant : public VulkanReflectedBlock {
    VkPushConstantRange _range;

public:
    /**
     * @brief Default constructor for VulkanReflectedPushConstant.
     */
    VulkanReflectedPushConstant() = default;

    /**
     * @brief Constructs a VulkanReflectedPushConstant with the given stages, offset, and size.
     * @param stages The shader stages this push constant is used in.
     * @param offset The offset in bytes of the push constant.
     * @param size The size in bytes of the push constant.
     */
    VulkanReflectedPushConstant(VkShaderStageFlags stages, uint32_t offset, uint32_t size);

    /**
     * @brief Default destructor for VulkanReflectedPushConstant.
     */
    ~VulkanReflectedPushConstant() = default;

    /**
     * @brief Sets the range of the push constant.
     * @param stages The shader stages this push constant is used in.
     * @param offset The offset in bytes of the push constant.
     * @param size The size in bytes of the push constant.
     */
    void SetRange(VkShaderStageFlags stages, uint32_t offset, uint32_t size);

    /**
     * @brief Adds shader stages to the push constant.
     * @param stages The shader stages to add.
     */
    void AddStageFlags(VkShaderStageFlags stages);

    /**
     * @brief Compares the offset and size of this push constant with another.
     * @param offset The offset to compare.
     * @param size The size to compare.
     * @return True if the offset and size match, false otherwise.
     */
    bool Compare(uint32_t offset, uint32_t size) const;

    /**
     * @brief Gets the Vulkan push constant range.
     * @return The Vulkan push constant range.
     */
    VkPushConstantRange GetRange() const;

    /**
     * @brief Gets the shader stages this push constant is used in.
     * @return The shader stages this push constant is used in.
     */
    VkShaderStageFlags GetStages() const;
};

} // namespace bl