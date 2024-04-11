#pragma once

#include "Device.h"
#include "PipelineResource.h"

namespace bl {

/** @brief A single unit of a shader pipeline. */
class BLUEMETAL_API GfxShader 
{
public:

    /** @brief Constructor */
    GfxShader(
        std::shared_ptr<GfxDevice>  device,
        VkShaderStageFlagBits       stage,     /** @brief Stage type i.e. Vertex, Fragment. */
        const std::vector<char>&    binary);   /** @brief Binary SPIR-V data from a shader file or resource. */

    /** @brief Destructor */
    ~GfxShader();

public:

    /** @brief Returns the underlying shader module handle. */
    VkShaderModule get() const;

    /** @brief Returns the shader stage created with. */
    VkShaderStageFlagBits getStage() const;

    /** @brief Returns vertex input bindings. */
    // VkVertexInputBindingDescription getVertexBinding();

    /** @brief Returns all vertex attribute descriptions from the shader. */
    // std::vector<VkVertexInputAttributeDescription> getVertexAttributes();

    /** @brief Returns uniforms from the shader. */
    // std::vector<GfxPipelineResource> getResources();

private:
    void createModule(const std::vector<char>& binary);
    // void createReflection(const std::vector<uint32_t>& binary);
    // void findVertexState();
    // void findResources();

    std::shared_ptr<GfxDevice>  _device;
    VkShaderStageFlagBits       _stage;
    VkShaderModule              _module;
    // SpvReflectShaderModule                          _reflection;
    // VkVertexInputBindingDescription                 _vertexBinding;
    // std::vector<VkVertexInputAttributeDescription>  _vertexAttributes;
    // std::vector<GfxPipelineResource>                _resources;
};

} // namespace bl
