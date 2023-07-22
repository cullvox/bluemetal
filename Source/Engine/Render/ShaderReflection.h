#pragma once

#include "Precompiled.h"
#include "Export.h"

namespace bl
{

struct BLUEMETAL_API DescriptorLayoutInfo
{
    std::string                                 name;
    uint32_t                                    set;
    std::vector<VkDescriptorSetLayoutBinding>   bindings;

    bool operator==(const DescriptorLayoutInfo& other) const;

    size_t hash() const;
};

class BLUEMETAL_API ShaderReflection
{
public:
    ShaderReflection(const std::vector<uint32_t>& code);
    ~ShaderReflection();

    VkShaderStageFlags getStage();
    VkVertexInputBindingDescription getVertexBinding(); // only applicable for vertex shader
    std::vector<VkVertexInputAttributeDescription> getVertexAttributes(); // only applicable for vertex shader
    void addDescriptorInfo(VkDescriptorSetLayoutBinding)

private:
    VkShaderStageFlagBits                           m_stage;
    VkVertexInputBindingDescription                 m_vertexBinding;
    std::vector<VkVertexInputAttributeDescription>  m_vertexAttributes;

};

} // namespace bl