#pragma once

#include "Device.h"
#include "Shader.h"
#include "DescriptorLayoutCache.h"
#include "RenderPass.h"
#include "PipelineResource.h"

namespace bl
{

/// A program consisting of multiple shaders that run on the GPU.
///
///     Underlying is a VkPipeline object that is constructed.
class BLUEMETAL_API Pipeline
{
public:

    /// Constructs a pipeline object.
    ///
    ///     @param pDevice Logical device used to create this pipeline. 
    ///     @param descriptorLayoutCache Speed up creation by caching descriptor set layouts.
    ///     @param shaders The shaders used in this pipeline program.
    ///     @param renderPass What pass this pipeline is used in.
    ///     @param subpass What subpass on the render pass is this used in.
    ///
    Pipeline(
        GraphicsDevice*                             pDevice, 
        DescriptorLayoutCache*                      descriptorLayoutCache, 
        const std::vector<Shader*>&                 shaders, 
        RenderPass*                                 renderPass, 
        uint32_t                                    subpass);

    /// Default Destructor
    ~Pipeline();

    /// Gets the raw VkPipelineLayout used to create this pipeline.
    VkPipelineLayout getPipelineLayout();

    /// Gets the raw VkPipeline underlying this object.
    VkPipeline getPipeline();

    /// Returns reflected resource info that this pipeline uses for materials and others. 
    ///
    /// Gets information for materials like descriptor set bindings.
    std::vector<PipelineResource> getResources();

    /// Binds this pipeline to a command buffer.
    ///
    ///     @param cmd Command buffer to bind on. 
    void bind(VkCommandBuffer cmd);

private:
    void getDescriptorLayouts(DescriptorLayoutCache* descriptorLayoutCache);
    void createLayout();
    void createPipeline(const std::vector<Shader*>& shaders);
    void mergeShaderResources(const std::vector<PipelineResource>& resources);

    GraphicsDevice*                         m_pDevice;
    std::shared_ptr<RenderPass>             m_pRenderPass;
    uint32_t                                m_subpass;
    std::map<std::string, PipelineResource> m_resources;
    std::vector<VkDescriptorSetLayout>      m_setLayouts;
    VkPipelineLayout                        m_pipelineLayout;
    VkPipeline                              m_pipeline;
    
};

} // namespace bl


/* Getting real with descriptor sets.

    Descriptor Slot: 0
        Reserved for global data of the frame.
        
        This Includes
            * Current Time
            * Camera MVP
            * Resolution
            * Texture Atlas
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