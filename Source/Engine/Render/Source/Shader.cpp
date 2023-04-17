#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/FormatConversions.hpp"
#include "Render/Shader.hpp"


blShader::blShader(std::shared_ptr<const blRenderDevice> renderDevice,
    std::span<uint32_t> code)
    : _renderDevice(renderDevice)
{
    const VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = code.size_bytes(),
        .pCode = code.data()
    };

    if (vkCreateShaderModule(
            _renderDevice->getDevice(), 
            &createInfo, 
            nullptr, 
            &_module) 
        != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan shader module!");
    }

    SpvReflectShaderModule reflectModule{};
    if (spvReflectCreateShaderModule(
            code.size_bytes(), 
            code.data(), 
            &reflectModule) 
        != SPV_REFLECT_RESULT_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan reflection shader module!");
    }

    _stage = (VkShaderStageFlagBits)reflectModule.shader_stage;

    findVertexState(reflectModule);

    spvReflectDestroyShaderModule(&reflectModule);
}

blShader::~blShader() noexcept
{
    vkDestroyShaderModule(_renderDevice->getDevice(), _module, nullptr);
} 

VkShaderStageFlagBits blShader::getStage() const noexcept
{
    return _stage;
}

VkShaderModule blShader::getModule() const noexcept
{
    return _module;
}

const VkPipelineVertexInputStateCreateInfo& blShader::getVertexState() const
{
    if (_stage != VK_SHADER_STAGE_VERTEX_BIT)
        throw std::runtime_error("Tried to get vertex state of a non vertex shader!");

    return _vertexState;
}

const std::vector<blShaderDescriptorReflection>& blShader::getDescriptorReflections() const noexcept
{
    return _descriptorReflections;
}

void blShader::findVertexState(const SpvReflectShaderModule& reflModule)
{

    if (_stage != VK_SHADER_STAGE_VERTEX_BIT)
        return;

    // These settings are all assumed until pointed otherwise needed to change.
    _vertexBinding.binding = 0;
    _vertexBinding.stride = 0; // Computed later
    _vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    for (uint32_t i = 0; i < reflModule.input_variable_count; i++)
    {
        const SpvReflectInterfaceVariable& reflVar = *(reflModule.input_variables[i]);
        
        // Skip built in variables
        if (reflVar.decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
        {
            continue;
        } 

        const VkVertexInputAttributeDescription attributeDescription{
            .location = reflVar.location,
            .binding = _vertexBinding.binding,
            .format = (VkFormat)reflVar.format,
            .offset = 0, // Computed later
        };

        _vertexAttributes.emplace_back(attributeDescription);
    }

    // Sort the attributes to be in location order
    std::sort(
        _vertexAttributes.begin(),
        _vertexAttributes.end(),
        [](const VkVertexInputAttributeDescription& a,
           const VkVertexInputAttributeDescription& b){
            return a.location < b.location;
        });

    // Compute the binding stride and attribute offsets
    for (auto& attribute : _vertexAttributes)
    {
        uint32_t formatSize = blVulkanFormat::getSize(attribute.format);

        attribute.offset = _vertexBinding.stride;
        _vertexBinding.stride += formatSize;
    }

    _vertexState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    _vertexState.pNext = nullptr;
    _vertexState.flags = 0;
    _vertexState.vertexBindingDescriptionCount = 1;
    _vertexState.pVertexBindingDescriptions = &_vertexBinding;
    _vertexState.vertexAttributeDescriptionCount = (uint32_t)_vertexAttributes.size();
    _vertexState.pVertexAttributeDescriptions = _vertexAttributes.data(); 

}

void blShader::findDescriptorReflections(const SpvReflectShaderModule& reflModule)
{

    _descriptorReflections.resize(reflModule.descriptor_set_count);

    for (size_t i = 0; i < _descriptorReflections.size(); i++)
    {
        const SpvReflectDescriptorSet& reflSet = reflModule.descriptor_sets[i];
        blShaderDescriptorReflection& reflSetData = _descriptorReflections[i];

        for (size_t j = 0; j < reflSet.binding_count; j++)
        {
            const SpvReflectDescriptorBinding& reflBinding = *(reflSet.bindings[j]);
            
            VkDescriptorSetLayoutBinding layoutBinding{
                .binding = reflBinding.binding,
                .descriptorType = (VkDescriptorType)reflBinding.descriptor_type,
                .descriptorCount = 1,
                .stageFlags = (VkShaderStageFlags)_stage,
                .pImmutableSamplers = nullptr,
            };

            for (uint32_t k = 0; k < reflBinding.array.dims_count; k++)
            {
                layoutBinding.descriptorCount *= reflBinding.array.dims[k];
            }

            reflSetData.bindings.emplace_back(layoutBinding);
        }

        reflSetData.setNumber = reflSet.set;
        reflSetData.info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        reflSetData.info.pNext = nullptr;
        reflSetData.info.flags = 0;
        reflSetData.info.bindingCount = reflSetData.bindings.size();
        reflSetData.info.pBindings = reflSetData.bindings.data();
    }

}