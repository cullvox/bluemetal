#pragma once

#include "Device.h"

struct blShaderDescriptorReflection
{
    uint32_t                                  set;
    VkDescriptorSetLayoutCreateInfo           info;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
};

class BLUEMETAL_API blShader
{
public:
    explicit blShader(std::shared_ptr<blDevice> device, const std::vector<uint32_t>& bytes);
    ~blShader() noexcept;

    VkShaderStageFlagBits getStage();
    VkShaderModule getModule();
    VkPipelineVertexInputStateCreateInfo getVertexState();
    std::vector<blShaderDescriptorReflection> getDescriptorReflections();

private:
    void findVertexState(const SpvReflectShaderModule* module);
    void findDescriptorReflections(const SpvReflectShaderModule* module);

    std::shared_ptr<blDevice>                       _renderDevice;
    VkShaderModule                                  _module;
    VkShaderStageFlagBits                           _stage;
    VkVertexInputBindingDescription                 _vertexBinding;
    std::vector<VkVertexInputAttributeDescription>  _vertexAttributes;
    VkPipelineVertexInputStateCreateInfo            _vertexState;
    std::vector<blShaderDescriptorReflection>       _descriptorReflections;
};
