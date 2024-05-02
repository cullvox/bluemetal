#include "ShaderReflection.h"
#include "Conversions.h"

namespace bl {

ShaderReflection::ShaderReflection(const std::vector<uint32_t>& binary)
{
    createReflection(binary);
    findVertexState();
    findResources();
}

ShaderReflection::~ShaderReflection() { spvReflectDestroyShaderModule(&m_reflection); }

bool ShaderReflection::getVertexBinding(VkVertexInputBindingDescription& inputBindingDesc)
{
    if (m_stage != VK_SHADER_STAGE_VERTEX_BIT) {
        blError("Tried to get vertex state of a non vertex shader!");
        return false;
    }

    inputBindingDesc = _vertexBinding;
    return true;
}

bool ShaderReflection::getVertexAttributes(std::vector<VkVertexInputAttributeDescription>& inputAttributeDesc)
{
    if (_stage != VK_SHADER_STAGE_VERTEX_BIT) {
        blError("Tried to get vertex state of a non vertex shader!");
        return false;
    }

    inputAttributeDesc = _vertexAttributes;
    return true;
}

bool ShaderReflection::createReflection(const std::vector<uint32_t>& binary) noexcept
{
    if (spvReflectCreateShaderModule(binary.size(), binary.data(), &_reflection) != SPV_REFLECT_RESULT_SUCCESS) {
        blError("Could not create a SpvReflectShaderModule!");
        return false;
    }

    _stage = (VkShaderStageFlagBits)_reflection.shader_stage;
    return true;
}

void ShaderReflection::findVertexState()
{
    if (_stage != VK_SHADER_STAGE_VERTEX_BIT) return;

    // Set the default vertex binding values.
    _vertexBinding.binding = 0;
    _vertexBinding.stride = 0; // computed
    _vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    // Get the vertex input variables.
    for (uint32_t i = 0; i < _reflection.input_variable_count; i++) {
        const SpvReflectInterfaceVariable* pVar = _reflection.input_variables[i];

        // Skip built in variables.
        if (pVar->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) {
            continue;
        }

        auto format = (VkFormat)pVar->format;

        // Create the attribute description.
        VkVertexInputAttributeDescription attribute = {};
        attribute.location = pVar->location;
        attribute.binding = 0;
        attribute.format = (VkFormat)pVar->format;
        attribute.offset = _vertexBinding.stride;

        // Compute the offset of this value.
        _vertexBinding.stride += vk::getFormatSize(format);

        _vertexAttributes.push_back(attribute);
    }

    // sort the attributes into location order
    std::sort(m_vertexAttributes.begin(), m_vertexAttributes.end(),
        [](const VkVertexInputAttributeDescription& a, const VkVertexInputAttributeDescription& b) {
            return a.location < b.location;
        });
}

void ShaderReflection::findResources()
{
    uint32_t bindingCount = 0;
    std::vector<SpvReflectDescriptorBinding*> bindings;
    spvReflectEnumerateDescriptorBindings(&_reflection, &bindingCount, nullptr);

    bindings.resize(bindingCount);

    spvReflectEnumerateDescriptorBindings(&_reflection, &bindingCount, bindings.data());

    for (const auto* b : bindings) {
        uint32_t count = 0;
        for (uint32_t k = 0; k < b->array.dims_count; k++) {
            count *= b->array.dims[k];
        }

        GfxPipelineResource resource = {};
        resource.stages = m_stage;
        resource.type = (VkDescriptorType)b->descriptor_type;
        // resources.access =
        resource.set = b->set;
        resource.binding = b->binding;
        resource.inputAttachmentIndex = b->input_attachment_index;
        // resource.vecSize = b->c;
        resource.arraySize = count;
        resource.name = std::string(b->name);

        _resources.emplace_back(resource);
    }
}

} // namespace bl
