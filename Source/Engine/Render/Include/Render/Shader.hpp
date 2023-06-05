#pragma once

#include "Core/Precompiled.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Precompiled.hpp"
#include "Render/Export.h"

struct blShaderDescriptorReflection
{
    uint32_t setNumber;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    VkDescriptorSetLayoutCreateInfo info;
};

class BLUEMETAL_RENDER_API blShader
{
public:
    blShader(std::shared_ptr<const blRenderDevice> renderDevice, std::span<const uint32_t> bytes);
    ~blShader() noexcept;

    vk::ShaderStageFlagBits getStage() const noexcept;
    vk::ShaderModule getModule() const noexcept;
    const vk::PipelineVertexInputStateCreateInfo& getVertexState() const; // Will throw if shader is not a vertex shader 
    const std::vector<blShaderDescriptorReflection>& getDescriptorReflections() const noexcept;

private:
    void findVertexState(const SpvReflectShaderModule& module);
    void findDescriptorReflections(const SpvReflectShaderModule& module);

    std::shared_ptr<const blRenderDevice> _renderDevice;

    vk::UniqueShaderModule                              _module;
    vk::ShaderStageFlagBits                             _stage;
    vk::VertexInputBindingDescription                   _vertexBinding;
    std::vector<vk::VertexInputAttributeDescription>    _vertexAttributes;
    vk::PipelineVertexInputStateCreateInfo              _vertexState;
    
    std::vector<blShaderDescriptorReflection>           _descriptorReflections;

};
