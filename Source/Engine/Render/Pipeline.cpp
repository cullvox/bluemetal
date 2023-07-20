#include "Pipeline.h"
#include "Core/Log.h"

blPipeline::blPipeline(std::shared_ptr<blDevice> device, std::shared_ptr<blDescriptorLayoutCache> cache, const std::vector<std::shared_ptr<blShader>>& shaders, std::shared_ptr<blRenderPass> renderPass, uint32_t subpass)
    : _device(device)
    , _cache(cache)
    , _renderPass(renderPass)
    , _subpass(subpass)
{
    createDescriptorSetLayouts(shaders);
    createLayout();
    createPipeline(shaders);
}

blPipeline::~blPipeline()
{
}

VkPipelineLayout blPipeline::getPipelineLayout()
{
    return _pipelineLayout;
}

VkPipeline blPipeline::getPipeline()
{
    return _pipeline;
}

void blPipeline::createDescriptorSetLayouts(std::shared_ptr<blDescriptorLayoutCache> layoutCache, const std::vector<std::shared_ptr<blShader>>& shaders)
{


    // create or find a descriptor layout in cache
    auto createLayout = [&](std::shared_ptr<blShader> shader)
    {

        shader->

        VkDescriptorSetLayoutCreateInfo createInfo =
        {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,    // sType
            nullptr,                                                // pNext
            0,                                                      // flags
            
            
        }

        layoutCache->createDescriptorLayout()

    }

    // Create the descriptor set layouts
    for (const auto& layout : layouts)
    {
        _setLayouts.push_back(_renderDevice.getDevice().createDescriptorSetLayoutUnique(layout.second));
    }
}

void blPipeline::createLayout()
{
    const VkPipelineLayoutCreateInfo createInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // sType
        nullptr,                                        // pNext
        0,                                              // flags
        (uint32_t)_setLayouts.size(),                   // setLayoutCount
        _setLayouts.data(),                             // pSetLayouts
        0,                                              // pushConstantRangeCount
        nullptr                                         // pPushConstantRanges
    };

    if (vkCreatePipelineLayout(_device->getDevice(), &createInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create the Vulkan pipeline layout!");
    }

}

void blPipeline::createPipeline(const std::vector<std::shared_ptr<blShader>>& shaders)
{
    std::shared_ptr<blShader> vertexShader;

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfos(shaders.size());
    
    std::transform(shaders.begin(), shaders.end(), shaderStageCreateInfos.begin(),
        [&](std::shared_ptr<blShader> shader)
        {
            // find the vertex shader to get vertex state
            if (shader->getStage() == vk::ShaderStageFlagBits::eVertex)
                vertexShader = shader;

            // build a pipeline shader info
            return vk::PipelineShaderStageCreateInfo
            {
                {},                     // flags
                shader->getStage(),     // stage
                shader->getModule(),    // module
                "main",                 // pName
                nullptr                 // pSpecializationInfo
            };
        });

    const vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = vertexShader->getVertexState();

    const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo
    {
        {},                                     // flags
        vk::PrimitiveTopology::eTriangleList,   // topology
        VK_TRUE                                 // primitiveRestartEnable
    };

    const vk::PipelineTessellationStateCreateInfo tessellationStateCreateInfo
    {
        {}, // flags 
        0   // patchControlPoints
    };

    const std::array viewports
    {
        vk::Viewport
        {
            0.0f, 0.0f, 
            0.0f, 0.0f,
            0.0f, 1.0f
        }
    };

    const std::array scissors
    {
        vk::Rect2D
        {
           { 0, 0 },
           { 0, 0 }
        }
    };

    const vk::PipelineViewportStateCreateInfo viewportStateCreateInfo
    {
        {},
        viewports,
        scissors
    };

    const vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo
    {
        {},                             // flags
        VK_FALSE,                       // depthClampEnable 
        VK_FALSE,                       // rasterizerDiscardEnable
        vk::PolygonMode::eFill,         // polygonMode
        vk::CullModeFlagBits::eBack,    // cullMode
        vk::FrontFace::eClockwise,      // frontFace
        VK_FALSE,                       // depthBiasEnable
        0.0f,                           // depthBiasConstantFactor
        0.0f,                           // depthBiasClamp
        0.0f,                           // depthBiasSlopeFactor
        1.0f                            // lineWidth
    };

    const vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo{
        {},                            // flags 
        vk::SampleCountFlagBits::e1,   // rasterizationSamples                             
        VK_FALSE,                      // sampleShadingEnable         
        1.0f,                          // minSampleShading     
        nullptr,                       // pSampleMask         
        VK_FALSE,                      // alphaToCoverageEnable         
        VK_FALSE,                      // alphaToOneEnable         
    };

    const vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo
    {
        // TODO 
    }; 

    const std::array attachments = 
    {
        vk::PipelineColorBlendAttachmentState{
            VK_TRUE,                            // blendEnable
            vk::BlendFactor::eSrcAlpha,         // srcColorBlendFactor        
            vk::BlendFactor::eOneMinusSrcAlpha, // dstColorBlendFactor                
            vk::BlendOp::eAdd,                  // colorBlendOp
            vk::BlendFactor::eOne,              // srcAlphaBlendFactor    
            vk::BlendFactor::eZero,             // dstAlphaBlendFactor    
            vk::BlendOp::eAdd,                  // alphaBlendOp
        }
    };

    const vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo
    {
        {},                             // flags
        VK_TRUE,                        // logicOpEnable
        vk::LogicOp::eCopy,             // logicOp
        attachments,                    // attachments
        { 0.0f, 0.0f, 0.0f, 0.0f }      // blendConstants
    };

    const std::array dynamicStates
    {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    const vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo
    {
        {},             // flags
        dynamicStates   // dynamicStates
    };

    const vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo
    {
        {},                             // flags
        shaderStageCreateInfos,         // stages
        &vertexInputStateCreateInfo,    // pVertexInputState
        &inputAssemblyStateCreateInfo,  // pInputAssemblyState
        &tessellationStateCreateInfo,   // pTessellationState
        &viewportStateCreateInfo,       // pViewportState
        &rasterizationStateCreateInfo,  // pRasterizationState
        &multisampleStateCreateInfo,    // pMultisampleState
        VK_NULL_HANDLE,                 // pDepthStencilState
        &colorBlendStateCreateInfo,     // pColorBlendState
        &dynamicStateCreateInfo,        // pDynamicState
        getPipelineLayout(),            // layout
        _renderPass.getRenderPass(),    // renderPass
        _subpass,                       // subpass
        VK_NULL_HANDLE,                 // basePipelineHandle
        0                               // basePipelineIndex
    };

    _pipeline = _renderDevice.getDevice().createGraphicsPipelineUnique(VK_NULL_HANDLE, graphicsPipelineCreateInfo).value;
}