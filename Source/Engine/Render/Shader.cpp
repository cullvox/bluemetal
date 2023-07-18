#include "Shader.h"
#include "FormatConversions.h"
#include "Core/Log.h"

blShader::blShader(std::shared_ptr<blDevice> device, const std::vector<uint32_t>& spirv)
    : _device(device)
{
    createShaderModule(spirv);
    reflect(spirv);
}

blShader::~blShader()
{
    vkDestroyShaderModule(_device->getDevice(), _module, nullptr);
} 

VkShaderStageFlagBits blShader::getStage()
{
    return _stage;
}

VkShaderModule blShader::getModule()
{
    return _module;
}

VkVertexInputBindingDescription blShader::getVertexBinding()
{
    if (_stage != VK_SHADER_STAGE_VERTEX_BIT)
    {
        throw std::runtime_error("Tried to get vertex state of a non vertex shader!");
    }

    return _vertexBinding;
}

std::vector<VkVertexInputAttributeDescription> blShader::getVertexAttributes()
{
    if (_stage != VK_SHADER_STAGE_VERTEX_BIT)
    {
        throw std::runtime_error("Tried to get vertex state of a non vertex shader!");
    }

    return _vertexAttributes;
}

std::vector<blShaderReflectDescriptorSet> blShader::getDescriptorSetReflections()
{
    return _descriptorSetReflections;
}

void blShader::createShaderModule(const std::vector<uint32_t>& spirv)
{
    const VkShaderModuleCreateInfo createInfo
    {
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,    // sType
        nullptr,                                        // pNext
        0,                                              // flags
        spirv.size(),                                   // codeSize
        spirv.data()                                    // pCode
    };

    if (vkCreateShaderModule(_device->getDevice(), &createInfo, nullptr, &_module) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create Vulkan shader module!");
    }
}

void blShader::reflect(const std::vector<uint32_t>& spirv)
{
    // preform reflection for materials and others
    SpvReflectShaderModule reflection{};
    if (spvReflectCreateShaderModule(spirv.size(), spirv.data(), &reflection) != SPV_REFLECT_RESULT_SUCCESS)
    {
        throw std::runtime_error("Could not create a SPIRV-reflect shader module!");
    }

    _stage = (VkShaderStageFlagBits)reflection.shader_stage;

    findVertexState(&reflection);
    findDescriptorReflections(&reflection);

    spvReflectDestroyShaderModule(&reflection);
}

void blShader::findVertexState(const SpvReflectShaderModule* reflection)
{
    if (_stage != VK_SHADER_STAGE_VERTEX_BIT) return;

    // these default vertex binding options are fine right now
    _vertexBinding.binding = 0;
    _vertexBinding.stride = 0; // computed
    _vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    // get the vertex inputs
    for (uint32_t i = 0; i < reflection->input_variable_count; i++)
    {
        const SpvReflectInterfaceVariable* variable = reflection->input_variables[i];
        
        // skip built in variables
        if (variable->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
        {
            continue;
        } 

        auto format = (VkFormat)variable->format;

        // create attrib description
        const VkVertexInputAttributeDescription attribute
        {
            variable->location,         // location
            0,                          // binding
            (VkFormat)variable->format, // format
            _vertexBinding.stride,      // offset
        };

        // compute offset
        _vertexBinding.stride += blVulkanFormat::getSize(format);

        _vertexAttributes.push_back(attribute);
    }

    // sort the attributes into location order
    std::sort(_vertexAttributes.begin(), _vertexAttributes.end(),
        [](const VkVertexInputAttributeDescription& a, const VkVertexInputAttributeDescription& b)
        {
            return a.location < b.location;
        });
}

void blShader::findDescriptorSetReflections(const SpvReflectShaderModule* reflection)
{
    size_t descriptorSetCount = reflection->descriptor_set_count;

    // create descriptor reflections
    _descriptorSetReflections.resize(descriptorSetCount);
 
    for (size_t i = 0; i < descriptorSetCount; i++)
    {
        // get the descriptor set 
        const SpvReflectDescriptorSet* descriptor = &reflection->descriptor_sets[i];

        // populate the list of bindings
        std::vector<VkDescriptorSetLayoutBinding> bindings(descriptor->binding_count);
        
        for (size_t j = 0; j < bindings.size(); j++)
        {
            const SpvReflectDescriptorBinding* binding = descriptor->bindings[j];
            
            // determine the count of multidimensional arrays 
            uint32_t count = 0;
            for (uint32_t k = 0; k < binding->array.dims_count; k++)
            {
                count *= binding->array.dims[k];
            }
            
            // create the reflected info
            blShaderReflectDescriptorBinding refl
            {
                binding->binding,                           // binding
                (VkDescriptorType)binding->descriptor_type  // type
                count,                                      // descriptorCount
                
            }


            bindings.push_back(layoutBinding);
        }
    }
}