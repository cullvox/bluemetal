#pragma once

#include <spirv_reflect.h>

#include "VulkanDevice.h"

namespace bl {

/** @brief A single unit of a shader pipeline. */
class VulkanShader {
    VulkanDevice* _device;
    VkShaderStageFlagBits _stage;
    SpvReflectShaderModule _reflect;
    VkShaderModule _module;

public:
    VulkanShader(VulkanDevice* device, std::span<uint32_t> data); /** @brief Constructor */
    ~VulkanShader(); /** @brief Destructor */

    VkShaderStageFlagBits GetStage() const; /** @brief Returns the shader stage created with. */
    const SpvReflectShaderModule& GetReflection() const; /** @brief Returns the reflection module. */
    VkShaderModule Get() const; /** @brief Returns the underlying shader module handle. */
};

} // namespace bl
