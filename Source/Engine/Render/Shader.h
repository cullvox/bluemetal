#pragma once

#include "Device.h"

struct blShaderReflectDescriptorBinding
{
    uint32_t            binding;
    VkDescriptorType    type;
    uint32_t            descriptorCount;
    const VkSampler*    pImmutableSamplers;
    std::string         name;
};

struct blShaderReflectDescriptorSet
{
    uint32_t set;
    std::string name;
    std::vector<blShaderReflectDescriptorBinding> bindings;
}

class BLUEMETAL_API blShader
{
public:
    explicit blShader(std::shared_ptr<blDevice> device, const std::vector<uint32_t>& spirv);
    ~blShader() noexcept;

    VkShaderStageFlagBits getStage();
    VkShaderModule getModule();
    VkVertexInputBindingDescription getVertexBinding();
    std::vector<VkVertexInputAttributeDescription> getVertexAttributes();
    void applyDescriptor(std::vector<VkDescriptorSetLayoutBinding>& bindings);
    std::vector<blShaderReflectDescriptorSet> getDescriptorSetReflections();

private:
    void createShaderModule(const std::vector<uint32_t>& spirv);
    void reflect(const std::vector<uint32_t>& spirv);
    void findVertexState(const SpvReflectShaderModule* module);
    void findDescriptorSetReflections(const SpvReflectShaderModule* module);

    std::shared_ptr<blDevice>                       _device;
    VkShaderModule                                  _module;
    VkShaderStageFlagBits                           _stage;
    VkVertexInputBindingDescription                 _vertexBinding;
    std::vector<VkVertexInputAttributeDescription>  _vertexAttributes;
    std::vector<blShaderReflectDescriptorSet>       _descriptorSetReflections;
};
