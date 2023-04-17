#include "Core/Log.hpp"
#include "Render/Pipeline.hpp"

#include <spirv_reflect.h>

blPipeline::blPipeline(std::shared_ptr<const blRenderDevice> renderDevice,
        std::vector<std::shared_ptr<const blShader>>& shaders,
        std::shared_ptr<const blRenderPass> renderPass,
        uint32_t subpass)
    : _renderDevice(renderDevice)
{
    createDescriptorSetLayouts(shaders);
    createLayout();
    createPipeline(renderPass, shaders, subpass);
}

blPipeline::~blPipeline() noexcept
{
}

void blPipeline::createDescriptorSetLayouts(
    const std::vector<std::shared_ptr<const blShader>>& shaders)
{

    // Create a combined descriptor set layout data
    std::map<int, vk::DescriptorSetLayoutCreateInfo> layouts; 
    
    for (const auto shader : shaders)
    {
        const auto& descriptors = shader->getDescriptorReflections();

        for (const auto& descriptor : descriptors)
        {
            
            // Check if this descriptor set number has already been used
            if (layouts.contains(descriptor.setNumber))
            {

                // A copy of the descriptor must be at that location, or somethings wrong!
                if (layouts[descriptor.setNumber] != descriptor.info)
                {
                    throw std::runtime_error("Descriptor used same set location, but different data!");
                }
            }

            // Add the descriptor because it hasn't been used before
            layouts[descriptor.setNumber] = descriptor.info;
        }
    }

    // Create the descriptor set layouts
    for (const auto& layout : layouts)
    {
        _setLayouts.push_back(
            _renderDevice->getDevice()
            .createDescriptorSetLayoutUnique(
                layout.second));
    }
}

void blPipeline::createLayout()
{
    const VkPipelineLayoutCreateInfo layoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    if (vkCreatePipelineLayout(
            _renderDevice->getDevice(), 
            &layoutInfo, 
            nullptr, 
            &_pipelineLayout)
        != VK_SUCCESS)
    {
        BL_LOG(blLogType::eFatal, "Could not create a pipeline layout!");
    }
}

void blPipeline::createPipeline(
    std::shared_ptr<const blRenderPass> renderPass,
    const std::vector<std::shared_ptr<const blShader>>& shaders,
    uint32_t subpass)
{

    std::vector<VkPipelineShaderStageCreateInfo> stages(shaders.size());
    VkPipelineVertexInputStateCreateInfo vertexState;

    for (int i = 0; i < shaders.size(); i++)
    {
        const auto shader = shaders[i];

        const VkPipelineShaderStageCreateInfo stageInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = shader->getStage(),
            .module = shader->getModule(),
            .pName = "main", // All shaders must use 'main' for entry 
            .pSpecializationInfo = nullptr
        };
        
        stages[i] = stageInfo;
    
        // If this shader is a vertex shader determine the vertex state
        if (shader->getStage() == VK_SHADER_STAGE_VERTEX_BIT)
        {
            vertexState = shader->getVertexState();
        }
    }

    const VkPipelineInputAssemblyStateCreateInfo inputAssembly{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_TRUE
    };

    const VkPipelineTessellationStateCreateInfo tessellationState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .patchControlPoints = 0
    };

    const std::array<vk::Viewport, 1> viewports{
        {
            0.0f, 0.0f, 
            0.0f, 0.0f,
            0.0f, 1.0f
        }
    };

    const std::array<vk::Scissor, 1> scissors{
        {
            
        }
    }

    const vk::PipelineViewportStateCreateInfo viewportState{
        {},
        {},
        {}    

        .viewportCount = 0,
        .pViewports = nullptr,
        .scissorCount = 0,
        .pScissors = nullptr
    };

    const VkPipelineRasterizationStateCreateInfo rasterizationState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f
    };

    const vk::PipelineMultisampleStateCreateInfo multisampleState{
        {},                            // flags 
        vk::SampleCountFlagBits::e1,   // rasterizationSamples                             
        VK_FALSE,                      // sampleShadingEnable         
        1.0f,                          // minSampleShading     
        nullptr,                       // pSampleMask         
        VK_FALSE,                      // alphaToCoverageEnable         
        VK_FALSE,                      // alphaToOneEnable         
    };

    const vk::PipelineDepthStencilStateCreateInfo depthStencilState{}; // TODO 

    const std::array<vk::PipelineColorBlendAttachmentState, 1> attachments = {
        {
            VK_TRUE,                                // blendEnable
            vk::BlendFactor::eSrcAlpha,             // srcColorBlendFactor        
            vk::BlendFactor::eOneMinusSrcAlpha,     // dstColorBlendFactor                
            vk::BlendOp::eAdd,                      // colorBlendOp
            vk::BlendFactor::eOne,                  // srcAlphaBlendFactor    
            vk::BlendFactor::eZero,                 // dstAlphaBlendFactor    
            vk::BlendOp::eAdd,                      // alphaBlendOp
        }
    };

    const vk::PipelineColorBlendStateCreateInfo colorBlendState{
        {},                             // flags
        VK_TRUE,                        // logicOpEnable
        vk::LogicOp::eCopy,             // logicOp
        attachments,                    // attachments
        { 0.0f, 0.0f, 0.0f, 0.0f }      // blendConstants
    };

    const std::array dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    const VkPipelineDynamicStateCreateInfo dynamicStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = dynamicStates.size(),
        .pDynamicStates = dynamicStates.data()
    };

    const vk::GraphicsPipelineCreateInfo pipelineInfo{
        {},                         // flags
        stages,                     // stages
        &vertexState,               // pVertexInputState
        &inputAssembly,             // pInputAssemblyState
        &tessellationState,         // pTessellationState
        &viewportState,             // pViewportState
        &rasterizationState,        // pRasterizationState
        &multisampleState,          // pMultisampleState
        nullptr,                    // pDepthStencilState
        &colorBlendState,           // pColorBlendState
        &dynamicStateInfo,          // pDynamicState
        _pipelineLayout,            // layout
        renderPass->getPass(),      // renderPass
        subpass,                    // subpass
        VK_NULL_HANDLE,             // basePipelineHandle
        0                           // basePipelineIndex
    };

    _renderDevice->getDevice()
        .createGraphicsPipelineUnique()

    if (vkCreateGraphicsPipelines(
            _renderDevice->getDevice(), 
            VK_NULL_HANDLE, 
            1, 
            &pipelineInfo, 
            nullptr, 
            &_pipeline)
        != VK_SUCCESS)
    {
        BL_LOG(blLogType::eFatal, "Could not create a vulkan graphics pipeline!");
    }
}