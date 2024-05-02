#pragma once

#include <spirv_reflect.h>

#include "Device.h"

namespace bl 
{

/** @brief A single unit of a shader pipeline. */
class Shader 
{
public:
    Shader(Device* device, VkShaderStageFlagBits stage, const std::vector<char>& binary);  /** @brief Constructor */
    ~Shader(); /** @brief Destructor */

public:
    VkShaderStageFlagBits GetStage() const; /** @brief Returns the shader stage created with. */
    const SpvReflectShaderModule& GetReflection() const; /** @brief Returns the reflection module. */
    VkShaderModule Get() const; /** @brief Returns the underlying shader module handle. */

private:
    Device* _device;
    VkShaderStageFlagBits _stage;
    SpvReflectShaderModule _reflect;
    VkShaderModule _module;
};

} // namespace bl
