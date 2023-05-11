#pragma once

#include "Render/Precompiled.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Shader.hpp"
#include "Render/RenderPass.hpp"

class BLUEMETAL_RENDER_API blPipeline
{

public:
    blPipeline(
        std::shared_ptr<const blRenderDevice> renderDevice,
        std::vector<std::shared_ptr<const blShader>>& shaders,
        std::shared_ptr<const blRenderPass> renderPass,
        uint32_t subpass = 0);
    ~blPipeline() noexcept;

    vk::PipelineLayout getPipelineLayout() const noexcept;
    vk::Pipeline getPipeline() const noexcept;
    const std::vector<vk::DescriptorSetLayout>& getDescriptorSetLayouts() const noexcept;

private:
    void createDescriptorSetLayouts(
        const std::vector<std::shared_ptr<const blShader>>& shaders);
    void createLayout();
    void createPipeline(std::shared_ptr<const blRenderPass> renderPass,
        const std::vector<std::shared_ptr<const blShader>>& shaders,
        uint32_t subpass);

    std::shared_ptr<const blRenderDevice> _renderDevice;

    std::vector<vk::UniqueDescriptorSetLayout> _setLayouts;
    vk::UniquePipelineLayout _pipelineLayout;
    vk::UniquePipeline _pipeline;
};


/* Getting real with descriptor sets.

    Descriptor Slot: 0
        Reserved for global data of the frame.
        
        This Includes
            * Current Time
            * Camera MVP
            * Resolution
            * ... (TBD)
    
    Descriptor Slot: 1
        Reserved for material data. Will change multiple times per frame. Each
        material instance's data will be uploaded here.

        Material data is custom to the shader and pipelines will agree to what
        ever the shader says because of reflection. The material system will 
        also take from the pipelines reflection and build a list of values that
        can be changed at runtime using this descriptor index.

        Pipelines are essentially here on the order.

    Descriptor Slot: 2
        Reserved for object data. This will change many times during the runtime
        of a singe frame. 

        This Includes:
            * Object/Mesh Transform
            * ... (TBD)

    Descriptor Slot: 3
        To be determined, might be for instanced data and/or textures?
        Instanced data might need to go on slot 2.

*/