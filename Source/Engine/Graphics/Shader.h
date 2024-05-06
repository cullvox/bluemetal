#pragma once

#include <spirv_reflect.h>

#include "Resource/Resource.h"
#include "Device.h"

namespace bl 
{

/** @brief A single unit of a shader pipeline. */
class Shader : public Resource
{
public:
    Shader(const nlohmann::json& json, Device* device);  /** @brief Constructor */
    ~Shader(); /** @brief Destructor */

    virtual void Load();
    virtual void Unload();

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
