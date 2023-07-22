#pragma once

#include "Device.h"

namespace bl
{

struct DescriptorLayoutInfo
{
    std::string                                 name;
    uint32_t                                    set;
    std::vector<VkDescriptorSetLayoutBinding>   bindings;

    bool operator==(const DescriptorLayoutInfo& other) const;

    size_t hash() const;
};

class BLUEMETAL_API ShaderReflection
{
    ShaderReflection(const std::vector<uint32_t>& spirv);
    ~ShaderReflection();

private:
    
};

class BLUEMETAL_API Shader
{
public:
    Shader(std::shared_ptr<Device> device, const std::vector<uint32_t>& spirv);
    ~Shader();

    VkShaderStageFlagBits getStage();
    VkShaderModule getModule();
    VkVertexInputBindingDescription getVertexBinding();
    std::vector<VkVertexInputAttributeDescription> getVertexAttributes();
    void applyDescriptor(std::vector<VkDescriptorSetLayoutBinding>& bindings);
    std::vector<DescriptorLayoutInfo> getDescriptorSetReflections();

private:
    void createShaderModule(const std::vector<uint32_t>& spirv);
    void reflect(const std::vector<uint32_t>& spirv);
    void findVertexState(const SpvReflectShaderModule& module);
    void findDescriptorSetLayoutInfo(const SpvReflectShaderModule& module);

    std::shared_ptr<Device>                         _device;
    VkShaderModule                                  _module;
    VkShaderStageFlagBits                           _stage;
    VkVertexInputBindingDescription                 _vertexBinding;
    std::vector<VkVertexInputAttributeDescription>  _vertexAttributes;
    std::vector<DescriptorLayoutInfo>               _descriptorSetReflections;
};

} // namespace bl