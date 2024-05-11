#include "Core/Print.h"
#include "Pipeline.h"

namespace bl 
{

Pipeline::Pipeline(Device* device, const PipelineCreateInfo& createInfo)
    : _device(device)
{
    Create(createInfo);
}

Pipeline::~Pipeline()
{ 
    vkDestroyPipeline(_device->Get(), _pipeline, nullptr);
}

VkPipelineLayout Pipeline::GetLayout() const 
{
    return _layout; 
}

VkPipeline Pipeline::Get() const 
{
    return _pipeline; 
}

// void Pipeline::mergeShaderResources(const std::vector<GfxPipelineResource>& shaderResources)
// {
//     for (auto& resource : shaderResources) {
//         // shader resources for input and outputs can have the same names, adding the stage name
//         // makes them unique
//         auto key = resource.name;
//         // if (resource.type  == PIPELINE_RESOURCE_TYPE_OUTPUT || resource.type ==
//         // PIPELINE_RESOURCE_TYPE_INPUT)
//         //    key = std::to_string(resource.stages) + ":" + key;
// 
//         // try to find the resource in the pipelines resources
//         auto it = _resources.find(key);
// 
//         // if the resource is found add this stage to it, else add a new resource
//         if (it != m_resources.end())
//             it->second.stages |= resource.stages;
//         else
//             _resources.emplace(key, resource);
//     }
// }

static inline bool CompareBindings(const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b)
{
    return a.binding == b.binding && a.descriptorType == b.descriptorType && a.descriptorCount == b.descriptorCount;
};

void Pipeline::Create(const PipelineCreateInfo& createInfo)
{
    std::vector<VkPipelineShaderStageCreateInfo> stages{};
    stages.reserve(createInfo.shaders.size());

    // Obtain reflection data from shaders to build the pipeline layout.
    std::unordered_map<uint32_t, DescriptorSetLayoutBindings> descriptorSetBindings;
    std::vector<VkPushConstantRange> pushConstantRanges;

    for (auto resource : createInfo.shaders)
    {
        auto shader = resource.Get();
        const auto& reflection = shader->GetReflection();

        // Build a pipeline shader stage.
        VkPipelineShaderStageCreateInfo stage{};
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.pNext = nullptr;
        stage.flags = 0;
        stage.stage = shader->GetStage();
        stage.module = shader->Get();
        stage.pName = "main";
        stage.pSpecializationInfo = nullptr;

        stages.push_back(stage);

        // Reflect each descriptor binding to build descriptor set layouts.
        for (uint32_t i = 0; i < reflection.descriptor_binding_count; i++)
        {
            const auto& db = reflection.descriptor_bindings[i];
            auto& set = descriptorSetBindings[db.set];

            VkDescriptorSetLayoutBinding binding{};
            binding.binding = db.binding;
            binding.descriptorType = (VkDescriptorType)db.descriptor_type;
            binding.descriptorCount = db.count;
            binding.stageFlags = shader->GetStage();
            binding.pImmutableSamplers = nullptr;

            // If this binding number already exists then compare and use that.
            auto it = std::find_if(set.bindings.begin(), set.bindings.end(), 
                [db](auto&& binding){ return db.binding == binding.binding; });

            if (it != set.bindings.end())
            {
                // The binding exists, make sure they are the same.
                auto& existingBinding = (*it);
                if (!CompareBindings(existingBinding, binding))
                    throw std::runtime_error("Bindings using same index but hold different types of data!");

                // Since they are the same just add this binding to the stage.
                existingBinding.stageFlags |= shader->GetStage();
            }
            else
            {
                // This binding doesn't exist yet for this descriptor, add it!
                set.bindings.push_back(binding);
            }
        }

        // Gather all the push constant ranges for the pipeline layout.
        for (uint32_t i = 0; i < reflection.push_constant_block_count; i++)
        {
            const auto& pcb = reflection.push_constant_blocks[i];

            VkPushConstantRange range{};
            range.size = pcb.size;
            range.offset = pcb.offset;
            range.stageFlags = shader->GetStage();

            // Check if the push constant already exists.
            auto it = std::find_if(pushConstantRanges.begin(), pushConstantRanges.end(), 
                [&range](auto&& existing){ return existing.size == range.size && existing.offset == range.offset; });

            if (it != pushConstantRanges.end())
            {
                // Add the stage to the existing push constant range.
                auto& existing = (*it);
                existing.stageFlags |= shader->GetStage();
            }
            else
            {
                // This block wasn't added yet.
                pushConstantRanges.push_back(range);
            }
        }
    }

    // Use the descriptor set layout cache to acquire layouts for each set.
    _descriptorSetLayouts.reserve(descriptorSetBindings.size());

    for (const auto& setBindings : descriptorSetBindings)
    {
        _descriptorSetLayouts.push_back(createInfo.setLayoutCache->Acquire(setBindings.second.bindings));
    }

    // Finally acquire our layout using the pipeline layout cache.
    _layout = createInfo.pipelineLayoutCache->Acquire(_descriptorSetLayouts, pushConstantRanges);

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.pNext = nullptr;
    vertexInputState.flags = 0;
    vertexInputState.vertexBindingDescriptionCount = (uint32_t)createInfo.vertexInputBindings.size();
    vertexInputState.pVertexBindingDescriptions = createInfo.vertexInputBindings.data();
    vertexInputState.vertexAttributeDescriptionCount = (uint32_t)createInfo.vertexInputAttribs.size();
    vertexInputState.pVertexAttributeDescriptions = createInfo.vertexInputAttribs.data();

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

    // VkPipelineDepthStencilStateCreateInfo depthStencilState = {};

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
        VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
        VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.flags = 0;
    dynamicState.dynamicStateCount = (uint32_t)dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pNext = nullptr;
    pipelineCreateInfo.flags = 0;
    pipelineCreateInfo.stageCount = (uint32_t)stages.size();
    pipelineCreateInfo.pStages = stages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pTessellationState = &tessellationState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = _layout;
    pipelineCreateInfo.renderPass = createInfo.renderPass->Get();
    pipelineCreateInfo.subpass = createInfo.subpass;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;

    VK_CHECK(vkCreateGraphicsPipelines(_device->Get(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_pipeline))
}

std::vector<VkDescriptorSetLayout> Pipeline::GetDescriptorSetLayouts() const
{
    return _descriptorSetLayouts;
}

} // namespace bl
