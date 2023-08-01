#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "PipelineResource.h"

namespace bl
{
class BLUEMETAL_API ShaderReflection
{
public:
    ShaderReflection(const std::vector<uint32_t>& binary);
    ~ShaderReflection();

    VkShaderStageFlags getStage();
    VkVertexInputBindingDescription getVertexBinding(); // only applicable for vertex shader
    std::vector<VkVertexInputAttributeDescription> getVertexAttributes(); // only applicable for vertex shader
    std::vector<PipelineResource> getResources(); // obtains the reflected uniforms

private:
    void createReflection(const std::vector<uint32_t>& binary);
    void findVertexState();
    void findResources();

    SpvReflectShaderModule                          m_reflection;
    VkShaderStageFlagBits                           m_stage;
    VkVertexInputBindingDescription                 m_vertexBinding;
    std::vector<VkVertexInputAttributeDescription>  m_vertexAttributes;
    std::vector<PipelineResource>                   m_resources;

};

} // namespace bl