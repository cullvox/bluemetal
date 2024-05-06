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

void Pipeline::Create(const PipelineCreateInfo& createInfo)
{
    std::vector<VkPipelineShaderStageCreateInfo> stages{};
    stages.reserve(createInfo.shaders.size());

    std::unordered_map<uint32_t, DescriptorSetLayoutBindings> pipelineSetBindings;
    for (auto resource : createInfo.shaders)
    {
        auto shader = resource.Get();
        const auto& reflection = shader->GetReflection();

        // Build this shaders sets and their bindings.
        std::unordered_map<uint32_t, DescriptorSetLayoutBindings> shaderSetBindings;
        for (uint32_t i = 0; i < reflection.descriptor_set_count; i++)
        {
            const auto& set = reflection.descriptor_sets[i];
            auto& bindings = shaderSetBindings[i].bindings; 
            bindings.reserve(set.binding_count);

            for (uint32_t j = 0; j < set.binding_count; j++)
            {
                const auto binding = set.bindings[j];
                bindings.emplace_back(j, (VkDescriptorType)binding->descriptor_type, binding->count, shader->GetStage(), nullptr);
            }
        }

        // Merge the sets and their bindings into the main pipeline bindings, check for discrepancies.
        auto compareBindings = [](const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b)
        {
            return a.binding == b.binding && a.descriptorType == b.descriptorType && a.descriptorCount == b.descriptorCount;
        };

        for (const auto& pair : shaderSetBindings)
        {
            uint32_t set = pair.first;
            auto& bindings = pair.second.bindings;

            if (pipelineSetBindings.contains(set))
            {
                auto& mainBindings = pipelineSetBindings[set].bindings;

                for (uint32_t i = 0; i < bindings.size(); i++)
                {
                    auto& binding = bindings[i];
                    auto bindingNum = binding.binding;

                    auto it = std::find_if(mainBindings.begin(), mainBindings.end(), [bindingNum](auto&& binding){ return binding.binding == bindingNum; });
                    if (it != mainBindings.end())
                    {
                        // Ensure that the bindings are the same in data.
                        if (compareBindings(binding, *it))
                        {
                            it->stageFlags |= shader->GetStage();
                        } 
                        else
                        {
                            throw std::runtime_error("Shader descriptor set layout does not match previous shader!");
                        }
                    }
                    else
                    {
                        // This binding wasn't added to the set yet and is new to this shader.
                        mainBindings.push_back(binding);
                    }
                }
            }
            else
            {
                // This set wasn't added to the pipeline layout yet, add it.
                pipelineSetBindings.emplace(set, bindings);
            }
        }
    }

    std::vector<VkDescriptorSetLayout> setLayouts;
    setLayouts.reserve(pipelineSetBindings.size());
    
    for (const auto& pair : pipelineSetBindings)
    {
        setLayouts.push_back(createInfo.setLayoutCache->acquire(pair.second.bindings));
    }

    _layout = createInfo.pipelineLayoutCache->acquire(setLayouts, {});

    std::transform(createInfo.shaders.begin(), createInfo.shaders.end(), std::back_inserter(stages), 
        [](auto&& resource){
            auto shader = resource.Get();
            VkPipelineShaderStageCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.flags = 0;
            createInfo.stage = shader->GetStage();
            createInfo.module = shader->Get();
            createInfo.pName = "main";
            createInfo.pSpecializationInfo = nullptr;
            return createInfo;
        });

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

    // The viewport state is set to dynamic, it doesn't matter.
    std::array<VkViewport, 1> viewports;
    viewports[0].x = 0.0f;
    viewports[0].y = 0.0f;
    viewports[0].width = 0.0f;
    viewports[0].height = 0.0f;
    viewports[0].minDepth = 0.0f;
    viewports[0].maxDepth = 1.0f;

    std::array<VkRect2D, 1> scissors;
    scissors[0].offset = { 0, 0 };
    scissors[0].extent = { 0, 0 };

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.flags = 0;
    viewportState.viewportCount = (uint32_t)viewports.size();
    viewportState.pViewports = viewports.data();
    viewportState.scissorCount = (uint32_t)scissors.size();
    viewportState.pScissors = scissors.data();

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
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
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
    pipelineCreateInfo.pDepthStencilState = VK_NULL_HANDLE;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = _layout;
    pipelineCreateInfo.renderPass = createInfo.renderPass->Get();
    pipelineCreateInfo.subpass = createInfo.subpass;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;

    VK_CHECK(vkCreateGraphicsPipelines(_device->Get(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &_pipeline))
}

} // namespace bl
