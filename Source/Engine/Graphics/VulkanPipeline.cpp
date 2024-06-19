#include "Core/Print.h"
#include "Graphics/VulkanBlockReflection.h"
#include "VulkanPipeline.h"
#include "vulkan/vulkan_core.h"

namespace bl {

VulkanPipelineReflection::VulkanPipelineReflection() = default;
VulkanPipelineReflection::VulkanPipelineReflection(const VulkanPipelineReflection& other) = default;
VulkanPipelineReflection::VulkanPipelineReflection(VulkanPipelineReflection&& other) = default;
VulkanPipelineReflection::VulkanPipelineReflection(const VulkanPipelineStateInfo& state) {
    std::array<VulkanShader*, 2> shaders = { state.stages.vert.Get(), state.stages.frag.Get() };

    // Obtain reflection data from shaders to build the pipeline layout.
    for (size_t i = 0; i < shaders.size(); i++) {

        // Reflect each descriptor binding to build descriptor set layouts.
        auto shader = shaders[i];
        auto reflection = shader->GetReflection();

        for (uint32_t j = 0; j < reflection.descriptor_binding_count; j++)
        {
            const auto& reflectBinding = reflection.descriptor_bindings[j];
            auto& set = _descriptorSetMetadata[reflectBinding.set];
            
            set.SetLocation(reflectBinding.set);

            // If this binding number already exists then compare and use that.
            auto location = reflectBinding.binding; 
            auto type = static_cast<VkDescriptorType>(reflectBinding.descriptor_type);
            auto count = reflectBinding.count;

            // If the set already contains this binding we do less work!
            bool doesBindingAlreadyExists = set.Contains(location);
            auto& binding = set[location];

            if (doesBindingAlreadyExists && !binding.Compare(type, count)) {
                throw std::runtime_error("Bindings using same index but hold different types of data!");
            } else {
                // This is a newly found binding
                binding.SetBinding(location, type, count, shader->GetStage(), nullptr);
                if (binding.IsBlock()) {
                    ReflectMembers(binding, location, reflectBinding.block);
                }
            }

            binding.AddStageFlags(shader->GetStage())
                .SetSize(reflectBinding.block.size);
        }

        // Gather all the push constant ranges for the pipeline layout.
        // Check if the push constant already exists.
        for (uint32_t j = 0; j < reflection.push_constant_block_count; j++)
        {
            const auto& block = reflection.push_constant_blocks[j];
            auto offset = block.offset; 
            auto size = block.size;

            auto it = std::find_if(_pushConstantMetadata.begin(), _pushConstantMetadata.end(),
                [offset, size](const auto& refl){ return refl.Compare(offset, size); });

            if (it != _pushConstantMetadata.end())
            {
                // Add the stage to the existing push constant range.
                auto& refl = (*it);
                refl.AddStageFlags(shader->GetStage());
            }
            else
            {
                // This block wasn't added yet.
                auto& pcm =_pushConstantMetadata.emplace_back(shader->GetStage(), offset, size);
                ReflectMembers(pcm, 0, block);
            }
        }
    }
}

VulkanPipelineReflection::~VulkanPipelineReflection() {}
VulkanPipelineReflection& VulkanPipelineReflection::operator=(const VulkanPipelineReflection& rhs) = default;
VulkanPipelineReflection& VulkanPipelineReflection::operator=(VulkanPipelineReflection&& rhs) = default;

std::map<uint32_t, VulkanDescriptorSetReflection>& VulkanPipelineReflection::GetDescriptorSetReflections() {
    return _descriptorSetMetadata;
}

std::vector<VulkanPushConstantReflection>& VulkanPipelineReflection::GetPushConstantReflections() {
    return _pushConstantMetadata;
}

const std::map<uint32_t, VulkanDescriptorSetReflection>& VulkanPipelineReflection::GetDescriptorSetReflections() const {
    return _descriptorSetMetadata;
}

const std::vector<VulkanPushConstantReflection>& VulkanPipelineReflection::GetPushConstantReflections() const {
    return _pushConstantMetadata;
}

void VulkanPipelineReflection::ReflectMembers(VulkanBlockReflection& meta, uint32_t binding, const SpvReflectBlockVariable& block, std::string parent)
{
    std::string structName;
    if (parent.empty()) {
        structName = block.name;
    } else {
        structName = fmt::format("{}.{}", parent, block.name);
    }

    meta.SetName(structName);

    for (uint32_t j = 0; j < block.member_count; j++)
    {
        auto& blockVariable = block.members[j];
        auto& numericTraits = blockVariable.numeric;
        auto typeDescription = blockVariable.type_description;
        VulkanBlockVariableType type;

        if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT) {
            ReflectMembers(meta, binding, blockVariable, meta.GetName());
            continue;
        }

        // We are a little specific about our supported material uniform block types.
        if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_ARRAY) {
            blWarning("Arrays are not supported in pipelines, it will not be parameterized.");
            continue;
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR) {

            // We only support floating vector types.
            if (!(typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT)) {
                blWarning("Only float vectors are supported in pipelines, {} in {} will not be parameterized.", typeDescription->struct_member_name, blockVariable.name);
                continue;
            }
            std::array types = { VulkanBlockVariableType::eVector2, VulkanBlockVariableType::eVector3, VulkanBlockVariableType::eVector4 };
            type = types[numericTraits.vector.component_count - 2];
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX) {

            // We only support 4x4 matrices. 
            if (numericTraits.matrix.column_count != 4 || numericTraits.matrix.row_count != 4) {
                blWarning("Only 4x4 matrices are supported in pipelines, {} in {} will not be parameterized.", typeDescription->struct_member_name, blockVariable.name);
                continue;
            }
            type = VulkanBlockVariableType::eMatrix4;
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_BOOL) {
            type = VulkanBlockVariableType::eScalarBool;
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_INT) {
            type = VulkanBlockVariableType::eScalarInt;
        } else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
            type = VulkanBlockVariableType::eScalarFloat;
        }

        std::string name = fmt::format("{}.{}", structName, blockVariable.name);
        meta[name].SetBinding(binding)
            .SetOffset(blockVariable.offset)
            .SetType(type)
            .SetSize(blockVariable.size)
            .SetName(name);
    }
}


Pipeline::Pipeline(VulkanDevice* device, const VulkanPipelineStateInfo& state, VkRenderPass pass, uint32_t subpass, const VulkanPipelineReflection* reflection)
    : _device(device) {
    

    _reflection = reflection ? *reflection : VulkanPipelineReflection{state};

    std::array<VkPipelineShaderStageCreateInfo, 2> stages;
    std::array<VulkanShader*, 2> shaders = { state.stages.vert.Get(), state.stages.frag.Get() };

    // Build the pipelines shader stage create info.
    for (size_t i = 0; i < shaders.size(); i++) {
        auto shader = shaders[i];
        stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[i].pNext = nullptr;
        stages[i].flags = 0;
        stages[i].stage = shader->GetStage();
        stages[i].module = shader->Get();
        stages[i].pName = "main";
        stages[i].pSpecializationInfo = nullptr;
    }

    const auto& descriptorSetMetadata = _reflection.GetDescriptorSetReflections();
    const auto& pushConstantMetadata = _reflection.GetPushConstantReflections();

    // Use the descriptor set layout cache to acquire layouts for each set.
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.reserve( descriptorSetMetadata.size());
    _descriptorSetLayouts.reserve(descriptorSetMetadata.size());

    // Extract descriptor set layout bindings and create a layout.
    for (auto& pair : descriptorSetMetadata) {
        auto& meta = pair.second;

        // Acquire a layout from cache or create a new descriptor set layout.
        auto sortedBindings = meta.GetSortedBindings();
        auto layout = _device->CacheDescriptorSetLayout(sortedBindings);
        _descriptorSetLayouts.emplace(meta.GetLocation(), layout);
        layouts.push_back(layout);
    }

    // Extract the push constant ranges from reflection.
    std::vector<VkPushConstantRange> pushConstants;
    pushConstants.reserve(pushConstantMetadata.size());

    for (const auto& meta : pushConstantMetadata) {
        pushConstants.push_back(meta.GetRange());
    }

    // Acquire our layout using the pipeline layout cache.
    _layout = _device->CachePipelineLayout(layouts, pushConstants);

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.pNext = nullptr;
    vertexInputState.flags = 0;
    vertexInputState.vertexBindingDescriptionCount = (uint32_t)state.vertexInput.vertexInputBindings.size();
    vertexInputState.pVertexBindingDescriptions = state.vertexInput.vertexInputBindings.data();
    vertexInputState.vertexAttributeDescriptionCount = (uint32_t)state.vertexInput.vertexInputAttribs.size();
    vertexInputState.pVertexAttributeDescriptions = state.vertexInput.vertexInputAttribs.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.pNext = nullptr;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    VkPipelineTessellationStateCreateInfo tessellationState = {};
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationState.pNext = nullptr;
    tessellationState.flags = 0;
    tessellationState.patchControlPoints = 0;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.flags = 0;
    viewportState.viewportCount = 0;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 0;
    viewportState.pScissors = nullptr;

    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.flags = 0;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.depthBiasConstantFactor = 0.0f;
    rasterizationState.depthBiasClamp = 0.0f;
    rasterizationState.depthBiasSlopeFactor = 0.0f;
    rasterizationState.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampleState = {};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.pNext = nullptr;
    multisampleState.flags = 0;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.minSampleShading = 1.0f;
    multisampleState.pSampleMask = nullptr;
    multisampleState.alphaToCoverageEnable = VK_FALSE;
    multisampleState.alphaToOneEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.pNext = nullptr;
    depthStencilState.flags = 0;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.front = {}; // unused
    depthStencilState.back = {};
    depthStencilState.minDepthBounds = 0.0f;
    depthStencilState.maxDepthBounds = 1.0f;

    std::array<VkPipelineColorBlendAttachmentState, 1> attachments = {};
    attachments[0].blendEnable = VK_TRUE;
    attachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    attachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    attachments[0].colorBlendOp = VK_BLEND_OP_ADD;
    attachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    attachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachments[0].alphaBlendOp = VK_BLEND_OP_ADD;
    attachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.pNext = nullptr;
    colorBlendState.flags = 0;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_COPY;
    colorBlendState.attachmentCount = (uint32_t)attachments.size();
    colorBlendState.pAttachments = attachments.data();
    colorBlendState.blendConstants[0] = 0.0f;
    colorBlendState.blendConstants[1] = 0.0f;
    colorBlendState.blendConstants[2] = 0.0f;
    colorBlendState.blendConstants[3] = 0.0f;

    std::array dynamicStates{
        VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT_EXT,
        VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT_EXT,
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.flags = {};
    dynamicState.dynamicStateCount = (uint32_t)dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.flags = {};
    pipelineCreateInfo.stageCount = (uint32_t)stages.size();
    pipelineCreateInfo.pStages = stages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pTessellationState = &tessellationState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = _layout;
    pipelineCreateInfo.renderPass = pass;
    pipelineCreateInfo.subpass = subpass;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; /* No vendor actually uses derivative pipelines. 😿 */
    pipelineCreateInfo.basePipelineIndex = 0;

    VK_CHECK(vkCreateGraphicsPipelines(_device->Get(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_pipeline))
}

Pipeline::~Pipeline()
{ 
    vkDestroyPipeline(_device->Get(), _pipeline, nullptr);
}

const VulkanPipelineReflection& Pipeline::GetReflection() const {
    return _reflection;
}

VkPipelineLayout Pipeline::GetLayout() const 
{
    return _layout; 
}

VkPipeline Pipeline::Get() const 
{
    return _pipeline; 
}

const std::unordered_map<uint32_t, VkDescriptorSetLayout>& Pipeline::GetDescriptorSetLayouts() const
{
    return _descriptorSetLayouts;
}


} // namespace bl
