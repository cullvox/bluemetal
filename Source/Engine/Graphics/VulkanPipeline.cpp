#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "Renderer.h"

#include "Core/Print.h"

namespace bl {

VulkanPipeline::VulkanPipeline(VulkanDevice* device, Renderer* renderer, const VulkanPipelineStateInfo& state, const VulkanReflectedPipeline* reflection)
    : _device(device)
{

    // Depending on circumstances the reflection of the pipeline can be edited by the user.
    // To enable this we don't instantaneously preform reflection, we check to see if the user
    // did it for us.+
    if (reflection)
        _reflection = *reflection;
    else
        _reflection = VulkanReflectedPipeline::Reflect(state.stages);

    std::vector<VkPipelineShaderStageCreateInfo> stages { state.stages.shaders.size() };
    std::vector<VulkanShader*> shaders = {};

    // Build the pipelines shader stage create info.
    for (size_t i = 0; i < state.stages.shaders.size(); i++) {
        auto shader = state.stages.shaders[i];
        stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[i].pNext = nullptr;
        stages[i].flags = 0;
        stages[i].stage = shader->GetStage();
        stages[i].module = shader->Get();
        stages[i].pName = "main";
        stages[i].pSpecializationInfo = nullptr;
    }

    const auto& descriptorSetMetadata = _reflection.descriptorSetMetadata;
    const auto& pushConstantMetadata = _reflection.pushConstantMetadata;

    // Use the descriptor set layout cache to acquire layouts for each set.
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.reserve(descriptorSetMetadata.size());
    // _descriptorSetLayouts.reserve(descriptorSetMetadata.size());

    // Extract descriptor set layout bindings and create a layout.
    for (auto& pair : descriptorSetMetadata) {
        auto& meta = pair.second;

        // Acquire a layout from cache or create a new descriptor set layout.
        auto sortedBindings = meta.GetSortedBindings();
        auto layout = device->AcquireDescriptorSetLayout(sortedBindings);
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
    _layout = _device->AcquirePipelineLayout(layouts, pushConstants);

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.pNext = nullptr;
    vertexInputState.flags = 0;
    vertexInputState.vertexBindingDescriptionCount = (uint32_t)state.vertexState.inputBindings.size();
    vertexInputState.pVertexBindingDescriptions = state.vertexState.inputBindings.data();
    vertexInputState.vertexAttributeDescriptionCount = (uint32_t)state.vertexState.inputAttribs.size();
    vertexInputState.pVertexAttributeDescriptions = state.vertexState.inputAttribs.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.pNext = nullptr;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = state.vertexState.topology;
    inputAssemblyState.primitiveRestartEnable = state.vertexState.primitiveRestartEnable;

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
    rasterizationState.depthClampEnable = state.rasterizerState.depthClampEnable;
    rasterizationState.rasterizerDiscardEnable = state.rasterizerState.rasterizerDiscardEnable;
    rasterizationState.polygonMode = state.rasterizerState.polygonMode;
    rasterizationState.cullMode = state.rasterizerState.cullMode;
    rasterizationState.frontFace = state.rasterizerState.frontFace;
    rasterizationState.depthBiasEnable = state.rasterizerState.depthBiasEnable;
    rasterizationState.depthBiasConstantFactor = state.rasterizerState.depthBiasConstantFactor;
    rasterizationState.depthBiasClamp = state.rasterizerState.depthBiasClamp;
    rasterizationState.depthBiasSlopeFactor = state.rasterizerState.depthBiasSlopeFactor;
    rasterizationState.lineWidth = state.rasterizerState.lineWidth;

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.pNext = nullptr;
    depthStencilState.flags = 0;
    depthStencilState.depthTestEnable = state.depthStencilState.depthTestEnable;
    depthStencilState.depthWriteEnable = state.depthStencilState.depthWriteEnable;
    depthStencilState.depthCompareOp = state.depthStencilState.depthCompareOp;
    depthStencilState.depthBoundsTestEnable = state.depthStencilState.depthBoundsTestEnable;
    depthStencilState.stencilTestEnable = state.depthStencilState.stencilTestEnable;
    depthStencilState.front = state.depthStencilState.front;
    depthStencilState.back = state.depthStencilState.back;
    depthStencilState.minDepthBounds = state.depthStencilState.minDepthBounds;
    depthStencilState.maxDepthBounds = state.depthStencilState.maxDepthBounds;


    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates = renderer->GetColorBlendAttachmentStates(state.pass);

    for (int i = 0; i < colorBlendAttachmentStates.size() && i < state.colorBlendState.attachments.size(); i++)
    {
        colorBlendAttachmentStates[i] = state.colorBlendState.attachments[i];
    }

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.pNext = nullptr;
    colorBlendState.flags = 0;
    colorBlendState.logicOpEnable = state.colorBlendState.logicOpEnable;
    colorBlendState.logicOp = state.colorBlendState.logicOp;
    colorBlendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
    colorBlendState.pAttachments = colorBlendAttachmentStates.data();
    colorBlendState.blendConstants[0] = state.colorBlendState.blendConstants[0];
    colorBlendState.blendConstants[1] = state.colorBlendState.blendConstants[1];
    colorBlendState.blendConstants[2] = state.colorBlendState.blendConstants[2];
    colorBlendState.blendConstants[3] = state.colorBlendState.blendConstants[3];

    auto dynamicStates = state.dynamicStates;
    dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT);
    dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT);

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = nullptr;
    dynamicState.flags = {};
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    auto colorAttachmentFormats = renderer->GetColorAttachmentFormats(state.pass);
    VkPipelineRenderingCreateInfo rendering = {};
    rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering.pNext = nullptr;
    rendering.viewMask = 0;
    rendering.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentFormats.size());
    rendering.pColorAttachmentFormats = colorAttachmentFormats.data();
    rendering.depthAttachmentFormat = renderer->GetDepthAttachmentFormat(state.pass);
    rendering.stencilAttachmentFormat = renderer->GetStencilAttachmentFormat(state.pass);

    std::vector<VkSampleCountFlagBits> multisampleCounts = renderer->GetMultisampleCounts();

    std::vector<VkPipelineMultisampleStateCreateInfo> multisampleStates;
    std::vector<VkGraphicsPipelineCreateInfo> pipelineCreateInfos;

    multisampleStates.reserve(multisampleCounts.size());
    pipelineCreateInfos.reserve(multisampleCounts.size());

    for (uint32_t i = 0; i < multisampleCounts.size(); i++)
    {
        VkPipelineMultisampleStateCreateInfo multisampleState = {};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.pNext = nullptr;
        multisampleState.flags = 0;
        multisampleState.rasterizationSamples = multisampleCounts[i];
        multisampleState.sampleShadingEnable = state.multisampleState.sampleShadingEnable;
        multisampleState.minSampleShading = state.multisampleState.minSampleShading;
        multisampleState.pSampleMask = nullptr;
        multisampleState.alphaToCoverageEnable = state.multisampleState.alphaToCoverageEnable;
        multisampleState.alphaToOneEnable = state.multisampleState.alphaToOneEnable;

        multisampleStates.push_back(multisampleState);

        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.pNext = &rendering;
        pipelineCreateInfo.flags = {};
        pipelineCreateInfo.stageCount = static_cast<uint32_t>(stages.size());
        pipelineCreateInfo.pStages = stages.data();
        pipelineCreateInfo.pVertexInputState = &vertexInputState;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineCreateInfo.pTessellationState = &tessellationState;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pRasterizationState = &rasterizationState;
        pipelineCreateInfo.pMultisampleState = &multisampleStates[i];
        pipelineCreateInfo.pDepthStencilState = &depthStencilState;
        pipelineCreateInfo.pColorBlendState = &colorBlendState;
        pipelineCreateInfo.pDynamicState = &dynamicState;
        pipelineCreateInfo.layout = _layout;
        pipelineCreateInfo.renderPass = VK_NULL_HANDLE; // Dynamic rendering.
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // No vendor actually uses derivative pipelines. (sad cat emoji)
        pipelineCreateInfo.basePipelineIndex = 0;

        pipelineCreateInfos.push_back(pipelineCreateInfo);
    }

    std::vector<VkPipeline> pipelines(pipelineCreateInfos.size());
    VK_CHECK(vkCreateGraphicsPipelines(_device->Get(), VK_NULL_HANDLE, static_cast<uint32_t>(pipelineCreateInfos.size()), pipelineCreateInfos.data(), nullptr, pipelines.data()))

    for (int i = 0; i < multisampleCounts.size(); i++)
    {
        _pipelines[multisampleCounts[i]] = pipelines[i];
    }

}

VulkanPipeline::~VulkanPipeline()
{
    for (auto [_, pipeline] : _pipelines)
    {
        vkDestroyPipeline(_device->Get(), pipeline, nullptr);
    }
}

VulkanPipeline& VulkanPipeline::operator=(VulkanPipeline&& move) noexcept
{
    _device = move._device;
    _reflection = std::move(move._reflection);
    _layout = move._layout;
    _pipelines = std::move(move._pipelines);
    _descriptorSetLayouts = std::move(move._descriptorSetLayouts);
    return *this;
}

const VulkanReflectedPipeline& VulkanPipeline::GetReflection() const
{
    return _reflection;
}

VkPipelineLayout VulkanPipeline::GetPipelineLayout() const
{
    return _layout;
}

VkPipeline VulkanPipeline::GetPipeline(VkSampleCountFlagBits multisampleCount) const
{
    uint32_t sc = static_cast<uint32_t>(multisampleCount);
    return _pipelines.at(sc);
}

const std::map<uint32_t, VkDescriptorSetLayout>& VulkanPipeline::GetDescriptorSetLayouts() const
{
    return _descriptorSetLayouts;
}

} // namespace bl
