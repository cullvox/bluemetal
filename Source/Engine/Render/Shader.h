#pragma once

#include "Device.h"

struct blDescriptorLayoutInfo
{
    std::string                                 name;
    uint32_t                                    set;
    std::vector<VkDescriptorSetLayoutBinding>   bindings;

    bool operator==(const blDescriptorLayoutInfo& other) const;

    size_t hash() const;
};


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
    std::vector<blDescriptorLayoutInfo> getDescriptorSetReflections();

private:
    void createShaderModule(const std::vector<uint32_t>& spirv);
    void reflect(const std::vector<uint32_t>& spirv);
    void findVertexState(const SpvReflectShaderModule& module);
    void findDescriptorSetLayoutInfo(const SpvReflectShaderModule& module);

    std::shared_ptr<blDevice>                       _device;
    VkShaderModule                                  _module;
    VkShaderStageFlagBits                           _stage;
    VkVertexInputBindingDescription                 _vertexBinding;
    std::vector<VkVertexInputAttributeDescription>  _vertexAttributes;
    std::vector<blDescriptorLayoutInfo>             _descriptorSetReflections;
};
