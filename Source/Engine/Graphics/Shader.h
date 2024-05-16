#pragma once

#include <spirv_reflect.h>

#include "Resource/Resource.h"
#include "Device.h"
#include "Descriptor.h"

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
    std::vector<DescriptorReflection> GetDescriptors(); /** @brief Returns descriptor set data extracted from reflection. */
    std::vector<VkPushConstantRange> GetPushConstantRanges() const;
    VkShaderModule Get() const; /** @brief Returns the underlying shader module handle. */

private:
    Device* _device;
    VkShaderStageFlagBits _stage;
    SpvReflectShaderModule _reflect;
    std::vector<DescriptorReflection> _descriptors;
    std::vector<VkPushConstantRange> _pushConstantRanges;
    VkShaderModule _module;
};

} // namespace bl
