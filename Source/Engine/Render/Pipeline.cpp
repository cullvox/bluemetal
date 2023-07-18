#include "Pipeline.h"
#include "Core/Log.h"

blPipeline::blPipeline(std::shared_ptr<blDevice> device, const std::vector<std::shared_ptr<blShader>>& shaders, std::shared_ptr<blRenderPass> renderPass, uint32_t subpass)
    : _device(device)
    , _renderPass(renderPass)
    , _subpass(subpass)
{
    createDescriptorSetLayouts(shaders);
    createLayout();
    createPipeline(shaders);
}

blPipeline::~blPipeline() noexcept
{
}

VkPipelineLayout blPipeline::getPipelineLayout() const noexcept
{
    return _pipelineLayout;
}

VkPipeline blPipeline::getPipeline() const noexcept
{
    return _pipeline;
}

void blPipeline::createDescriptorSetLayouts(const std::vector<std::shared_ptr<blShader>>& shaders)
{

    // create a combined descriptor set layout data
    std::map<int, VkDescriptorSetLayoutCreateInfo> layouts; 
    
    for (const auto& shader : shaders)
    {
        const auto& descriptors = shader->getDescriptorReflections();

        for (const auto& descriptor : descriptors)
        {
            
            // Check if this descriptor set number has already been used
            if (layouts.contains(descriptor.set))
            {

                // A copy of the descriptor must be at that location, or somethings wrong!
                if (layouts[descriptor.set] != descriptor.info)
                {
                    throw std::runtime_error("Descriptor used same set location, but different data!");
                }
            }

            // Add the descriptor because it hasn't been used before
            layouts[descriptor.set] = descriptor.info;
        }
    }

    // Create the descriptor set layouts
    for (const auto& layout : layouts)
    {
        _setLayouts.push_back(_renderDevice.getDevice().createDescriptorSetLayoutUnique(layout.second));
    }
}

void blPipeline::createLayout()
{
    // use transform to get not unique versions
    std::vector<vk::DescriptorSetLayout> setLayouts{};

    std::transform(_setLayouts.begin(), _setLayouts.end(), setLayouts.begin(), [](vk::UniqueDescriptorSetLayout& unique) { return unique.get(); });

    const vk::PipelineLayoutCreateInfo layoutInfo
    {
        {},
        setLayouts
    };

    _pipelineLayout = _renderDevice.getDevice().createPipelineLayoutUnique(layoutInfo);
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