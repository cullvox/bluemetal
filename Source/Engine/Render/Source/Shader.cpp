#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/FormatConversions.hpp"
#include "Render/Shader.hpp"


blShader::blShader(std::shared_ptr<const blRenderDevice> renderDevice,
    std::span<const uint32_t> code)
    : _renderDevice(renderDevice)
{
    
    const vk::ShaderModuleCreateInfo createInfo
    {
        {},     // flags
        code    // code
    };

    _module = _renderDevice->getDevice()
        .createShaderModuleUnique(createInfo);


    // Reflection on shader code using spirv-reflect
    SpvReflectShaderModule reflectModule{};
    if (spvReflectCreateShaderModule(
            code.size_bytes(), 
            code.data(), 
            &reflectModule) 
        != SPV_REFLECT_RESULT_SUCCESS)
    {
        throw std::runtime_error("Could not create a vulkan reflection shader module!");
    }

    _stage = (vk::ShaderStageFlagBits)reflectModule.shader_stage;

    findVertexState(reflectModule);
    findDescriptorReflections(reflectModule);

    spvReflectDestroyShaderModule(&reflectModule);
}

blShader::~blShader() noexcept
{
} 

vk::ShaderStageFlagBits blShader::getStage() const noexcept
{
    return _stage;
}

vk::ShaderModule blShader::getModule() const noexcept
{
    return _module.get();
}

const vk::PipelineVertexInputStateCreateInfo& blShader::getVertexState() const
{
    if (_stage != vk::ShaderStageFlagBits::eVertex)
        throw std::runtime_error("Tried to get vertex state of a non vertex shader!");

    return _vertexState;
}

const std::vector<blShaderDescriptorReflection>& blShader::getDescriptorReflections() const noexcept
{
    return _descriptorReflections;
}

void blShader::findVertexState(const SpvReflectShaderModule& reflModule)
{

    if (_stage != vk::ShaderStageFlagBits::eVertex)
        return;

    // These settings are all assumed until pointed otherwise needed to change.
    _vertexBinding.binding = 0;
    _vertexBinding.stride = 0; // Computed later
    _vertexBinding.inputRate = vk::VertexInputRate::eVertex;

    for (uint32_t i = 0; i < reflModule.input_variable_count; i++)
    {
        const SpvReflectInterfaceVariable& reflVar = *(reflModule.input_variables[i]);
        
        // Skip built in variables
        if (reflVar.decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
        {
            continue;
        } 

        const vk::VertexInputAttributeDescription attributeDescription{
            reflVar.location,               // location
            _vertexBinding.binding,         // binding
            (vk::Format)reflVar.format,     // format
            0,                              // offset (Computed later)
        };

        _vertexAttributes.emplace_back(attributeDescription);
    }

    // Sort the attributes to be in location order
    std::sort(
        _vertexAttributes.begin(),
        _vertexAttributes.end(),
        [](const vk::VertexInputAttributeDescription& a,
           const vk::VertexInputAttributeDescription& b){
            return a.location < b.location;
        });

    // Compute the binding stride and attribute offsets
    for (auto& attribute : _vertexAttributes)
    {
        uint32_t formatSize = blVulkanFormat::getSize(attribute.format);

        attribute.offset = _vertexBinding.stride;
        _vertexBinding.stride += formatSize;
    }

    _vertexState
        .setVertexBindingDescriptions({_vertexBinding})
        .setVertexAttributeDescriptions(_vertexAttributes); 
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
            
            vk::DescriptorSetLayoutBinding layoutBinding{
                reflBinding.binding,                                // binding
                (vk::DescriptorType)reflBinding.descriptor_type,    // descriptorType
                1,                                                  // descriptorCount
                (vk::ShaderStageFlags)_stage,                       // stageFlags
                nullptr,                                            // pImmutableSamplers
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