#pragma once

#include "Core/Precompiled.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Precompiled.hpp"

class blShader
{
public:
    blShader(const blRenderDevice* renderDevice,
        const std::vector<uint32_t>& spirvBytes);
    ~blShader() noexcept;

    std::vector<VkDescriptorSetLayoutBinding>& getDescriptorBindings() const noexcept;
    std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const noexcept;

private:
    bool generateReflectShaderModules() noexcept;
    void generateDescriptorBindings() noexcept;
    void generateShaderStages() noexcept;

    const blRenderDevice* _pRenderDevice;
    std::vector<std::filesystem::path> m_shaderPaths;
    std::vector<SpvReflectShaderModule> m_reflectShaderModules;
    std::vector<VkDescriptorSetLayoutBinding> m_descriptorBindings;
    std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
};
