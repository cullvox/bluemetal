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

vk::PipelineLayout blPipeline::getPipelineLayout() const noexcept
{
    return _pipelineLayout.get();
}

vk::Pipeline blPipeline::getPipeline() const noexcept
{
    return _pipeline.get();
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
    // Transform the vk::UniqueDescriptorSetLayout objects to normal vk::DescriptorSetLayout objects 
    std::vector<vk::DescriptorSetLayout> setLayouts{};
    std::transform(
        _setLayouts.begin(), _setLayouts.end(),
        setLayouts.begin(),
        [](vk::UniqueDescriptorSetLayout& unique){
            return unique.get();
        });

    const vk::PipelineLayoutCreateInfo layoutInfo{
        {},
        setLayouts,
        {},
        {}
    };

    _pipelineLayout = _renderDevice->getDevice()
        .createPipelineLayoutUnique(layoutInfo);
}

void blPipeline::createPipeline(
    std::shared_ptr<const blRenderPass> renderPass,
    const std::vector<std::shared_ptr<const blShader>>& shaders,
    uint32_t subpass)
{
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStageCreateInfos(shaders.size());
    
    std::transform(
        shaders.begin(), shaders.end(),
        shaderStageCreateInfos.begin(),
        [](std::shared_ptr<const blShader> shader)
        {
            return vk::PipelineShaderStageCreateInfo
            {
                {},                     // flags
                shader->getStage(),     // stage
                shader->getModule(),    // module
                "main",                 // pName
                nullptr                 // pSpecializationInfo
            };
        });

    auto vertexShader = 
        *std::find_if( // <- This dereference may be dangerous, but it does throw on failure
            shaders.begin(), shaders.end(), 
            [](auto shader)
            { 
                return shader->getStage() == vk::ShaderStageFlagBits::eVertex; 
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
        nullptr,                        // pDepthStencilState
        &colorBlendStateCreateInfo,     // pColorBlendState
        &dynamicStateCreateInfo,        // pDynamicState
        getPipelineLayout(),            // layout
        renderPass->getRenderPass(),    // renderPass
        subpass,                        // subpass
        VK_NULL_HANDLE,                 // basePipelineHandle
        0                               // basePipelineIndex
    };

    _pipeline = _renderDevice->getDevice()
        .createGraphicsPipelineUnique(VK_NULL_HANDLE, graphicsPipelineCreateInfo)
        .value;
}