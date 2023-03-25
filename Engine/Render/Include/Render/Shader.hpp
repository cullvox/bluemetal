#pragma once

#include "Render/RenderDevice.hpp"

#include <spirv_reflect.h>

#include <vector>
#include <filesystem>

namespace bl 
{

class Shader 
{
public:
    Shader() noexcept;
    Shader(RenderDevice& renderDevice, std::vector<std::filesystem::path> paths) noexcept;
    ~Shader() noexcept;

    Shader& operator=(Shader&& rhs) noexcept;

    bool good() const noexcept;
    std::vector<VkDescriptorSetLayoutBinding>& getDescriptorBindings() const noexcept;
    std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const noexcept;

private:
    void collapse() noexcept;
    bool generateReflectShaderModules() noexcept;
    void generateDescriptorBindings() noexcept;
    void generateShaderStages() noexcept;

    RenderDevice* m_pRenderDevice;
    std::vector<std::filesystem::path> m_shaderPaths;
    std::vector<SpvReflectShaderModule> m_reflectShaderModules;
    std::vector<VkDescriptorSetLayoutBinding> m_descriptorBindings;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
};

} // namespace bl