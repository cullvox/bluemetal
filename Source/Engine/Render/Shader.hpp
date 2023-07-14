#pragma once

#include "Core/Precompiled.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Precompiled.hpp"
#include "Render/Export.h"

struct blShaderDescriptorReflection
{
    uint32_t                                    set;
    vk::DescriptorSetLayoutCreateInfo           info;
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
};

class BLUEMETAL_RENDER_API blShader
{
public:
    blShader(blRenderDevice& renderDevice, std::span<uint32_t> bytes);
    ~blShader() noexcept;

    vk::ShaderStageFlagBits getStage() const noexcept;
    vk::ShaderModule getModule() const noexcept;
    vk::PipelineVertexInputStateCreateInfo getVertexState() const; // Will throw if shader is not a vertex shader 
    std::vector<blShaderDescriptorReflection> getDescriptorReflections() const noexcept;

private:
    void findVertexState(const SpvReflectShaderModule* module);
    void findDescriptorReflections(const SpvReflectShaderModule* module);

    blRenderDevice&                                     _renderDevice;
    vk::UniqueShaderModule                              _module;
    vk::ShaderStageFlagBits                             _stage;
    vk::VertexInputBindingDescription                   _vertexBinding;
    std::vector<vk::VertexInputAttributeDescription>    _vertexAttributes;
    vk::PipelineVertexInputStateCreateInfo              _vertexState;
    std::vector<blShaderDescriptorReflection>           _descriptorReflections;
};
