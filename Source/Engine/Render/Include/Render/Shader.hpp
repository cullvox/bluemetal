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

class BLOODLUST_RENDER_API blShader
{
public:
    blShader(std::shared_ptr<const blRenderDevice> renderDevice, 
        std::span<uint32_t> bytes);
    ~blShader() noexcept;

    VkShaderStageFlagBits getStage() const noexcept;
    VkShaderModule getModule() const noexcept;
    const VkPipelineVertexInputStateCreateInfo& getVertexState() const; // Will throw if shader is not a vertex shader 
    const std::vector<blShaderDescriptorReflection>& getDescriptorReflections() const noexcept;

private:
    void findVertexState(const SpvReflectShaderModule& module);
    void findDescriptorReflections(const SpvReflectShaderModule& module);

    std::shared_ptr<const blRenderDevice> _renderDevice;

    VkShaderModule _module;
    VkShaderStageFlagBits _stage;
    VkVertexInputBindingDescription _vertexBinding;
    std::vector<VkVertexInputAttributeDescription> _vertexAttributes;
    VkPipelineVertexInputStateCreateInfo _vertexState;
    std::vector<blShaderDescriptorReflection> _descriptorReflections;

};
