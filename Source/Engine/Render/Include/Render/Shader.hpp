#pragma once

#include "Core/Precompiled.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Precompiled.hpp"

class blShader
{
public:
    blShader(const std::shared_ptr<blRenderDevice> renderDevice, 
        std::span<uint32_t> bytes);
    ~blShader() noexcept;

    std::vector<VkDescriptorSetLayoutBinding>& getDescriptorBindings() const noexcept;
    std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const noexcept;

private:
    bool generateReflectShaderModules() noexcept;
    void generateDescriptorBindings() noexcept;
    void generateShaderStages() noexcept;

    const std::shared_ptr<blRenderDevice> _renderDevice;

    VkShaderModule _shaderModule;

    std::vector<SpvReflectShaderModule> _reflectShaderModules;
    std::vector<VkDescriptorSetLayoutBinding> _descriptorBindings;
};
