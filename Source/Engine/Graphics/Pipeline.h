#pragma once

#include "Device.h"
#include "Shader.h"
#include "DescriptorLayoutCache.h"
#include "RenderPass.h"
#include "PipelineResource.h"

namespace bl
{

/** @brief Create info for pipeline objects. */
struct PipelineCreateInfo {
    GraphicsDevice*           pDevice;                   /** @brief Logical device used to create this pipeline.  */
    DescriptorLayoutCache*    pDescriptorLayoutCache;    /** @brief Speed up creation by caching descriptor set layouts. */
    RenderPass*               pRenderPass;               /** @brief The shaders used in this pipeline program. */
    uint32_t                  subpass;                   /** @brief What pass this pipeline is used in. */
    std::vector<Shader*>      shaders;                   /** @brief What subpass on the render pass is this used in. */
};

/** @brief A program consisting of multiple shaders that run on the GPU. */
class BLUEMETAL_API Pipeline {
public:

    /** @brief Default Constructor */
    Pipeline();

    /** @brief Move Constructor */
    Pipeline(Pipeline&& rhs);

    /** @brief Constructs the pipeline by calling @ref create().     */
    Pipeline(const PipelineCreateInfo& info);

    /** @brief Default Destructor */
    ~Pipeline();

    /** @brief Move Operator */
    Pipeline& operator=(Pipeline&& rhs) noexcept;

    /** @brief Creates a pipeline object. */
    bool create(const PipelineCreateInfo& info);

    /** @brief Destroys a pipeline object. */
    void destroy() noexcept;

    /** @brief Gets the current created status. */
    bool isCreated() const noexcept;

public:

    /** @brief Gets the raw VkPipelineLayout used to create this pipeline. */
    VkPipelineLayout getLayout() const;

    /** @brief Gets the raw VkPipeline underlying this object. */
    VkPipeline getHandle() const;

    /** @brief Returns reflected resource info that this pipeline uses for materials and others. */
    std::vector<PipelineResource> getResources();

    /** @brief Binds this pipeline to a command buffer. */
    void bind(VkCommandBuffer cmd);

private:
    void getDescriptorLayouts(DescriptorLayoutCache* descriptorLayoutCache);
    bool createLayout();
    bool createPipeline(const std::vector<Shader*>& shaders);
    void mergeShaderResources(const std::vector<PipelineResource>& resources);

    std::string                             m_err;
    GraphicsDevice*                         m_pDevice;
    RenderPass*                             m_pRenderPass;
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
