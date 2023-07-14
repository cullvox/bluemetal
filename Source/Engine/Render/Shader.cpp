#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/FormatConversions.hpp"
#include "Render/Shader.hpp"


blShader::blShader(blRenderDevice& renderDevice, std::span<uint32_t> code)
    : _renderDevice(renderDevice)
{
    
    const vk::ShaderModuleCreateInfo createInfo
    {
        {},     // flags
        code    // code
    };

    _module = _renderDevice.getDevice().createShaderModuleUnique(createInfo);

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

    findVertexState(&reflectModule);
    findDescriptorReflections(&reflectModule);

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

vk::PipelineVertexInputStateCreateInfo blShader::getVertexState() const
{
    if (_stage != vk::ShaderStageFlagBits::eVertex)
    {
        throw std::runtime_error("Tried to get vertex state of a non vertex shader!");
    }

    return _vertexState;
}

std::vector<blShaderDescriptorReflection> blShader::getDescriptorReflections() const noexcept
{
    return _descriptorReflections;
}

void blShader::findVertexState(const SpvReflectShaderModule* spvModule)
{
    if (_stage != vk::ShaderStageFlagBits::eVertex) return;

    // these default vertex binding options are fine right now
    _vertexBinding.binding = 0;
    _vertexBinding.stride = 0; // computed later
    _vertexBinding.inputRate = vk::VertexInputRate::eVertex;

    for (uint32_t i = 0; i < spvModule->input_variable_count; i++)
    {
        const SpvReflectInterfaceVariable* spvVar = spvModule->input_variables[i];
        
        // skip built in variables
        if (spvVar->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
        {
            continue;
        } 

        // create attrib description
        const vk::VertexInputAttributeDescription attrib
        {
            spvVar->location,               // location
            _vertexBinding.binding,         // binding
            (vk::Format)spvVar->format,     // format
            0                               // offset (computed later)
        };

        _vertexAttributes.emplace_back(attrib);
    }

    // sort the attributes into location order
    std::sort(_vertexAttributes.begin(), _vertexAttributes.end(),
        [](const vk::VertexInputAttributeDescription& a,
           const vk::VertexInputAttributeDescription& b)
        {
            return a.location < b.location;
        });

    // compute the binding stride and attribute offsets
    for (auto& attribute : _vertexAttributes)
    {
        uint32_t formatSize = blVulkanFormat::getSize(attribute.format);

        attribute.offset = _vertexBinding.stride;
        _vertexBinding.stride += formatSize;
    }

    const std::array vertexBindings = { _vertexBinding };

    _vertexState
        .setVertexBindingDescriptions(vertexBindings)
        .setVertexAttributeDescriptions(_vertexAttributes); 
}

void blShader::findDescriptorReflections(const SpvReflectShaderModule* spvModule)
{
    size_t descriptorSetCount = spvModule->descriptor_set_count;

    // create descriptor reflections
    _descriptorReflections.resize(descriptorSetCount);
 
    for (size_t i = 0; i < descriptorSetCount; i++)
    {
        const SpvReflectDescriptorSet& spvSet = spvModule->descriptor_sets[i];
        blShaderDescriptorReflection& refSetData = _descriptorReflections[i];

        for (size_t j = 0; j < spvSet.binding_count; j++)
        {
            const SpvReflectDescriptorBinding* spvBinding = spvSet.bindings[j];
            
            // create the descriptor set layout bindings
            vk::DescriptorSetLayoutBinding layoutBinding
            {
                spvBinding->binding,                                // binding
                (vk::DescriptorType)spvBinding->descriptor_type,    // descriptorType
                1,                                                  // descriptorCount
                (vk::ShaderStageFlags)_stage,                       // stageFlags
                nullptr,                                            // pImmutableSamplers
            };

            // determine the count of multidimensional arrays for descriptorCount 
            for (uint32_t k = 0; k < spvBinding->array.dims_count; k++)
            {
                layoutBinding.descriptorCount *= spvBinding->array.dims[k];
            }

            refSetData.bindings.emplace_back(layoutBinding);
        }

        refSetData.set = spvSet.set;
        refSetData.info.setBindings(refSetData.bindings);
    }
}