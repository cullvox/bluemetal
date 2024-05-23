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

std::vector<DescriptorSetReflection> Pipeline::GetDescriptorSetReflections() const
{
    return _descriptorSetReflections;
}

std::vector<PushConstantReflection> Pipeline::GetPushConstantReflections() const
{
    return _pushConstantReflections;
}

void Pipeline::ReflectMembers(BlockReflection& builder, const SpvReflectBlockVariable& block)
{
    const UniformMemberType vectorTypes[4][3] = // [components][type]
    {
        {UniformMemberType::eScalarBool, UniformMemberType::eScalarInt, UniformMemberType::eScalarFloat},
        {UniformMemberType::eVector2b, UniformMemberType::eVector2i, UniformMemberType::eVector2f},
        {UniformMemberType::eVector3b, UniformMemberType::eVector3i, UniformMemberType::eVector3f},
        {UniformMemberType::eVector4b, UniformMemberType::eVector4i, UniformMemberType::eVector4f},
    };

    const UniformMemberType matrixTypes[3] =
    {
        UniformMemberType::eMatrix2f, UniformMemberType::eMatrix3f, UniformMemberType::eMatrix4f
    };

    for (uint32_t j = 0; j < block.member_count; j++)
    {
        auto& blockVariable = block.members[j];
        auto& numericTraits = blockVariable.numeric;
        auto typeDescription = blockVariable.type_description;

        BlockMemberReflection member{};
        member.name = blockVariable.name;
        member.offset = blockVariable.offset;
        member.size = blockVariable.size;

        if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_ARRAY)
        {
            continue; // We don't support array values just yet. (eventually for voxels!)
        }
        else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR)
        {
            uint32_t components = numericTraits.vector.component_count;
            uint32_t type = 0;
            if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_BOOL) type = 0; 
            else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_INT) type = 1;
            else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) type = 2;
            member.type = vectorTypes[components][type];
        }
        else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX)
        { 
            auto columns = numericTraits.matrix.column_count;
            auto rows = numericTraits.matrix.row_count;
            if (columns == rows) member.type = matrixTypes[columns];
            else { continue; } // We don't support matrices that aren't dimensionally equal.
        }
        else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_BOOL)
        {
            member.type = UniformMemberType::eScalarBool;
        }
        else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_INT)
        {
            member.type = UniformMemberType::eScalarInt;
        }
        else if (typeDescription->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT)
        {
            member.type = UniformMemberType::eScalarFloat;
        }

        builder[member.name] = member;
    }
}

void Pipeline::Create(const PipelineCreateInfo& createInfo)
{
    std::vector<VkPipelineShaderStageCreateInfo> stages{};
    stages.reserve(createInfo.shaders.size());

    // Obtain reflection data from shaders to build the pipeline layout.
    std::unordered_map<uint32_t, DescriptorSetReflection> descriptorSetReflections;
    std::vector<PushConstantReflection> pushConstantReflections;

    for (auto resource : createInfo.shaders)
    {
        auto shader = resource.Get();
        auto spvReflection = shader->GetReflection();

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
        for (uint32_t i = 0; i < spvReflection.descriptor_binding_count; i++)
        {
            const auto& spvBinding = spvReflection.descriptor_bindings[i];
            auto& reflection = descriptorSetReflections[spvBinding.set];
            reflection.SetLocation(spvBinding.set);

            // If this binding number already exists then compare and use that.
            auto location = spvBinding.binding; 
            auto type = (VkDescriptorType)spvBinding.descriptor_type;
            auto count = spvBinding.count;

            if (reflection.ContainsBinding(location))
            {
                // A binding has already been created, ensure it's the same and add this stage to it.
                auto& binding = reflection.AccessOrInsertBinding(location);
                if (!binding.Compare(type, count))
                    throw std::runtime_error("Bindings using same index but hold different types of data!");

                binding.AddStageFlags(shader->GetStage());
            }
            else
            {
                // Insert a new binding and set it's data.
                auto& binding = reflection.AccessOrInsertBinding(location);
                binding.SetBinding(location, type, count, shader->GetStage(), nullptr);

                // Reflect any and all members that this binding may have (if it's a uniform/storage buffer).
                const auto& block = spvBinding.block;
                ReflectMembers(binding, block);
            }
        }

        // Gather all the push constant ranges for the pipeline layout.
        // Check if the push constant already exists.
        for (uint32_t i = 0; i < spvReflection.push_constant_block_count; i++)
        {
            const auto& block = spvReflection.push_constant_blocks[i];
            auto offset = block.offset; 
            auto size = block.size;

            auto it = std::find_if(pushConstantReflections.begin(), pushConstantReflections.end(),
                [offset, size](const auto& refl){ return refl.Compare(offset, size); });

            if (it != pushConstantReflections.end())
            {
                // Add the stage to the existing push constant range.
                auto& refl = (*it);
                refl.AddStageFlags(shader->GetStage());
            }
            else
            {
                // This block wasn't added yet.
                _pushConstantReflections.emplace_back(shader->GetStage(), offset, size);
                ReflectMembers(_pushConstantReflections.back(), block);
            }
        }
    }

    // Use the descriptor set layout cache to acquire layouts for each set.
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.reserve(descriptorSetReflections.size());
    _descriptorSetReflections.reserve(descriptorSetReflections.size());

    // Extract descriptor set layout bindings and create a layout.
    for (auto& pair : descriptorSetReflections)
    {
        auto& reflection = pair.second;

        // Retrieve a layout from cache or create a new descriptor set layout.
        auto layout = _device->CacheDescriptorSetLayout(reflection.GetSortedBindings());
        reflection.SetLayout(layout); // We save these layouts in the reflection data too for ease of use.
        layouts.push_back(layout);

        // Save the reflection data into the final vector.
        _descriptorSetReflections.push_back(std::move(reflection));
    }

    // Extract the push constant ranges from reflection.
    std::vector<VkPushConstantRange> pushConstants;
    pushConstants.reserve(_pushConstantReflections.size());

    for (const auto& reflection : _pushConstantReflections)
        pushConstants.push_back(reflection.GetRange());

    // Acquire our layout using the pipeline layout cache.
    _layout = _device->CachePipelineLayout(layouts, pushConstants);

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
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; /* no vendor actually supports derivative pipelines. */
    pipelineCreateInfo.basePipelineIndex = 0;

    VK_CHECK(vkCreateGraphicsPipelines(_device->Get(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_pipeline))
}

} // namespace bl
