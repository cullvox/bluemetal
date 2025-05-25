#pragma once

#include <spirv_reflect.h>
#include <nlohmann/json.hpp>

#include "Resource/Resource.h"
#include "VulkanDevice.h"

namespace bl  {

/** @brief A single unit of a shader pipeline. */
class VulkanShader : public Resource {
public:
    VulkanShader(ResourceManager* manager, const nlohmann::json& json, VulkanDevice* device);  /** @brief Constructor */
    ~VulkanShader(); /** @brief Destructor */

    virtual void Load() override;
    virtual void Unload() override;

    VkShaderStageFlagBits GetStage() const; /** @brief Returns the shader stage created with. */
    const SpvReflectShaderModule& GetReflection() const; /** @brief Returns the reflection module. */
    VkShaderModule Get() const; /** @brief Returns the underlying shader module handle. */

private:
    VulkanDevice* _device;
    VkShaderStageFlagBits _stage;
    SpvReflectShaderModule _reflect;
    VkShaderModule _module;
};

} // namespace bl
