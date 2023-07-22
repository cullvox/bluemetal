#include "Shader.h"
#include "FormatConversions.h"
#include "Core/Log.h"

namespace bl
{

Shader::Shader(std::shared_ptr<Device> device, const std::vector<uint32_t>& spirv)
    : _device(device)
{
    createShaderModule(spirv);
    reflect(spirv);
}

Shader::~Shader()
{
    vkDestroyShaderModule(_device->getDevice(), _module, nullptr);
} 

VkShaderStageFlagBits Shader::getStage()
{
    return _stage;
}

VkShaderModule Shader::getModule()
{
    return _module;
}

VkVertexInputBindingDescription Shader::getVertexBinding()
{
    if (_stage != VK_SHADER_STAGE_VERTEX_BIT)
    {
        throw std::runtime_error("Tried to get vertex state of a non vertex shader!");
    }

    return _vertexBinding;
}

std::vector<VkVertexInputAttributeDescription> Shader::getVertexAttributes()
{
    if (_stage != VK_SHADER_STAGE_VERTEX_BIT)
    {
        throw std::runtime_error("Tried to get vertex state of a non vertex shader!");
    }

    return _vertexAttributes;
}

std::vector<DescriptorLayoutInfo> Shader::getDescriptorSetReflections()
{
    return _descriptorSetReflections;
}

void Shader::createShaderModule(const std::vector<uint32_t>& spirv)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = spirv.size();
    createInfo.pCode = spirv.data();

    if (vkCreateShaderModule(_device->getDevice(), &createInfo, nullptr, &_module) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create Vulkan shader module!");
    }
}

void Shader::reflect(const std::vector<uint32_t>& spirv)
{
    // preform reflection for materials and others
    SpvReflectShaderModule reflection{};
    if (spvReflectCreateShaderModule(spirv.size(), spirv.data(), &reflection) != SPV_REFLECT_RESULT_SUCCESS)
    {
        throw std::runtime_error("Could not create a SPIRV-reflect shader module!");
    }

    _stage = (VkShaderStageFlagBits)reflection.shader_stage;

    findVertexState(reflection);
    findDescriptorSetLayoutInfo(reflection);

    spvReflectDestroyShaderModule(&reflection);
}

void Shader::findVertexState(const SpvReflectShaderModule& reflection)
{
    if (_stage != VK_SHADER_STAGE_VERTEX_BIT) return;

    // these default vertex binding options are fine right now
    _vertexBinding.binding = 0;
    _vertexBinding.stride = 0; // computed
    _vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    // get the vertex inputs
    for (uint32_t i = 0; i < reflection.input_variable_count; i++)
    {
        const SpvReflectInterfaceVariable* variable = reflection.input_variables[i];
        
        // skip built in variables
        if (variable->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
        {
            continue;
        } 

        auto format = (VkFormat)variable->format;

        // create attrib description
        VkVertexInputAttributeDescription attribute = {};
        attribute.location = variable->location;
        attribute.binding = 0;
        attribute.format = (VkFormat)variable->format;
        attribute.offset = _vertexBinding.stride;

        // compute offset
        _vertexBinding.stride += VulkanFormat::getSize(format);

        _vertexAttributes.push_back(attribute);
    }

    // sort the attributes into location order
    std::sort(_vertexAttributes.begin(), _vertexAttributes.end(),
        [](const VkVertexInputAttributeDescription& a, const VkVertexInputAttributeDescription& b)
        {
            return a.location < b.location;
        });
}

void Shader::findDescriptorSetLayoutInfo(const SpvReflectShaderModule& reflection)
{
    size_t descriptorSetCount = reflection.descriptor_set_count;

    // create descriptor reflections
    _descriptorSetReflections.resize(descriptorSetCount);
 
    for (size_t i = 0; i < descriptorSetCount; i++)
    {
        // get the descriptor set 
        const SpvReflectDescriptorSet* descriptor = &reflection.descriptor_sets[i];

        // populate the list of bindings
        std::vector<VkDescriptorSetLayoutBinding> bindings(descriptor->binding_count);
        
        for (size_t j = 0; j < bindings.size(); j++)
        {
            const SpvReflectDescriptorBinding* spvBinding = descriptor->bindings[j];
            
            // determine the count of multidimensional arrays 
            uint32_t count = 0;
            for (uint32_t k = 0; k < spvBinding->array.dims_count; k++)
            {
                count *= spvBinding->array.dims[k];
            }
            
            // create the reflected info
            /*VkDescriptorSetLayoutBinding binding
            {
                spvBinding->binding,                            // binding
                (VkDescriptorType)spvBinding->descriptor_type   // descriptorType
                count,                                          // descriptorCount
                
            }*/


            // bindings.push_back(layoutBinding);
        }
    }
}

} // namespace bl