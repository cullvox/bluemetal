#include "Pipeline.h"
#include "Core/Log.h"
#include "ShaderReflection.h"

namespace bl
{

Pipeline:: Pipeline(std::shared_ptr<Device> device, std::shared_ptr<DescriptorLayoutCache> cache, std::vector<std::shared_ptr<Shader>> shaders, std::shared_ptr<RenderPass> renderPass, uint32_t subpass)
    : m_device(device)
    , m_renderPass(renderPass)
    , m_subpass(subpass)
{
    createLayout();
    createPipeline(shaders);
}

Pipeline::~Pipeline()
{
    vkDestroyPipeline(m_device->getHandle(), m_pipeline, nullptr);
}

VkPipelineLayout Pipeline::getPipelineLayout()
{
    return m_pipelineLayout;
}

VkPipeline Pipeline::getPipeline()
{
    return m_pipeline;
}

void Pipeline::mergeShaderResources(const std::vector<PipelineResource>& shaderResources)
{
    for (auto& resource : shaderResources)
    {
        // shader resources for input and outputs can have the same names, adding the stage name makes them unique
        auto key = resource.name;
        if (resource.type == PIPELINE_RESOURCE_TYPE_OUTPUT || resource.type == PIPELINE_RESOURCE_TYPE_INPUT)
            key = std::to_string(resource.stages) + ":" + key;
        
        // try to find the resource in the pipelines resources
        auto it = m_resources.find(key);

        // if the resource is found add this stage to it, else add a new resource
        if (it != m_resources.end())
            it->second.stages |= resource.stages;
        else
            m_resources.emplace(key, resource);
    }
}

void Pipeline::createDescriptorSetLayouts(std::shared_ptr<DescriptorLayoutCache> layoutCache, std::vector<std::shared_ptr<Shader>> shaders)
{


    // Create the descriptor set layouts
    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;

    std::vector<std::shared_ptr<Shader>> shaders;

    for (auto shader : shaders)
    {
        ShaderReflection reflection(shader->getBinary());  

        DescriptorLayoutInfo info;

        for (auto& resource : reflection.getResources())
        {
            if (resource.type != VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT && resource.type != VK_DESCRIPTOR_TYPE_OU)

            info.set = resource.set;
            info.bindings = resource.binding
            createInfo.bindingCount = 

            m_setLayouts.push_back(_renderDevice.getDevice().createDescriptorSetLayoutUnique(layout.second));
        }
    }
}

void Pipeline::getDescriptorLayouts(std::shared_ptr<DescriptorLayoutCache> descriptorLayoutCache)
{

    VkDescriptorSetLayoutCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.bindingCount = ;
    createInfo.pBindings = ;

    descriptorLayoutCache->createDescriptorLayout()
}

void Pipeline::createLayout()
{
    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.setLayoutCount = (uint32_t)m_setLayouts.size();
    createInfo.pSetLayouts = m_setLayouts.data();
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(m_device->getHandle(), &createInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create the Vulkan pipeline layout!");
    }

}

void Pipeline::createPipeline(std::vector<std::shared_ptr<Shader>> shaders)
{
    for (std::shared_ptr<Shader> shader : shaders)
    {
        shader->
    }

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages;
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].pNext = nullptr;
    shaderStages[0].flags = 0;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertexShader->getModule();
    shaderStages[0].pName = "main";
    shaderStages[0].pSpecializationInfo = nullptr;

    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].pNext = nullptr;
    shaderStages[1].flags = 0;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragmentShader->getModule();
    shaderStages[1].pName = "main";
    shaderStages[1].pSpecializationInfo = nullptr;

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.pNext = nullptr;
    vertexInputState.flags = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.pNext = nullptr;
    inputAssemblyState.flags = 0;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_TRUE;

    VkPipelineTessellationStateCreateInfo tessellationState = {};
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationState.pNext = nullptr;
    tessellationState.flags = 0;
    tessellationState.patchControlPoints = 0;

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

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};

    std::array<VkPipelineColorBlendAttachmentState, 1> attachments = {};
    attachments[0].blendEnable = VK_TRUE;
    attachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    attachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    attachments[0].colorBlendOp = VK_BLEND_OP_ADD;
    attachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    attachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachments[0].alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.pNext = nullptr;
    colorBlendState.flags = 0;
    colorBlendState.logicOpEnable = VK_TRUE;
    colorBlendState.logicOp = VK_LOGIC_OP_COPY;
    colorBlendState.attachmentCount = (uint32_t)attachments.size();
    colorBlendState.pAttachments = attachments.data();
    colorBlendState.blendConstants[0] = 0.0f;
    colorBlendState.blendConstants[1] = 0.0f;
    colorBlendState.blendConstants[2] = 0.0f;
    colorBlendState.blendConstants[3] = 0.0f;

    std::array dynamicStates
    {
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
    pipelineCreateInfo.stageCount = (uint32_t)shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pTessellationState = &tessellationState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pDepthStencilState = VK_NULL_HANDLE;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = m_pipelineLayout;
    pipelineCreateInfo.renderPass = m_renderPass->getHandle();
    pipelineCreateInfo.subpass = m_subpass;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;

    if (vkCreateGraphicsPipelines(m_device->getHandle(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create a Vulkan graphics pipeline!");
    }
}

} // namespace bl