#include "ShaderReflection.h"
#include "FormatConversions.h"

namespace bl
{

ShaderReflection::ShaderReflection(const std::vector<uint32_t>& binary)
{
    createReflection(binary);
    findVertexState();
    findResources();
}

ShaderReflection::~ShaderReflection()
{
    spvReflectDestroyShaderModule(&m_reflection);
}

VkVertexInputBindingDescription ShaderReflection::getVertexBinding()
{
    if (m_stage != VK_SHADER_STAGE_VERTEX_BIT)
    {
        throw std::runtime_error("Tried to get vertex state of a non vertex shader!");
    }

    return m_vertexBinding;
}

std::vector<VkVertexInputAttributeDescription> ShaderReflection::getVertexAttributes()
{
    if (m_stage != VK_SHADER_STAGE_VERTEX_BIT)
    {
        throw std::runtime_error("Tried to get vertex state of a non vertex shader!");
    }

    return m_vertexAttributes;
}

void ShaderReflection::createReflection(const std::vector<uint32_t>& binary)
{
    if (spvReflectCreateShaderModule(binary.size(), binary.data(), &m_reflection) != SPV_REFLECT_RESULT_SUCCESS)
    {
        throw std::runtime_error("Could not create a SPIRV-reflect shader module!");
    }

    m_stage = (VkShaderStageFlagBits)m_reflection.shader_stage;
}

void ShaderReflection::findVertexState()
{
    if (m_stage != VK_SHADER_STAGE_VERTEX_BIT) return;

    // these default vertex binding options are fine right now
    m_vertexBinding.binding = 0;
    m_vertexBinding.stride = 0; // computed
    m_vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    // get the vertex inputs
    for (uint32_t i = 0; i < m_reflection.input_variable_count; i++)
    {
        const SpvReflectInterfaceVariable* v = m_reflection.input_variables[i];
        
        // skip built in variables
        if (v->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN)
        {
            continue;
        } 

        auto format = (VkFormat)v->format;

        // create attrib description
        VkVertexInputAttributeDescription attribute = {};
        attribute.location = v->location;
        attribute.binding = 0;
        attribute.format = (VkFormat)v->format;
        attribute.offset = m_vertexBinding.stride;

        // compute offset
        m_vertexBinding.stride += vk::getFormatSize(format);

        m_vertexAttributes.push_back(attribute);
    }

    // sort the attributes into location order
    std::sort(m_vertexAttributes.begin(), m_vertexAttributes.end(),
        [](const VkVertexInputAttributeDescription& a, const VkVertexInputAttributeDescription& b)
        {
            return a.location < b.location;
        });
}

void ShaderReflection::findResources()
{
    uint32_t bindingCount = 0;
    std::vector<SpvReflectDescriptorBinding*> bindings;
    spvReflectEnumerateDescriptorBindings(&m_reflection, &bindingCount, nullptr);

    bindings.resize(bindingCount);

    spvReflectEnumerateDescriptorBindings(&m_reflection, &bindingCount, bindings.data());

    for (const auto* b : bindings)
    {
        uint32_t count = 0;
        for (uint32_t k = 0; k < b->array.dims_count; k++)
        {
            count *= b->array.dims[k];
        }

        PipelineResource resource = {};
        resource.stages = m_stage;
        resource.type = (VkDescriptorType)b->descriptor_type;
        // resources.access =
        resource.set = b->set;
        resource.binding = b->binding;
        resource.inputAttachmentIndex = b->input_attachment_index;
        // resource.vecSize = b->c;
        resource.arraySize = count;
        resource.name = std::string(b->name);

        m_resources.emplace_back(resource);
    }
}

} // namespace bl