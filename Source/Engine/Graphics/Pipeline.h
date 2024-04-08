#pragma once

#include "Device.h"
#include "Shader.h"
#include "DescriptorLayoutCache.h"
#include "RenderPass.h"
#include "PipelineResource.h"

namespace bl {

/** @brief A program consisting of shaders designed to run on the GPU. */
class BLUEMETAL_API GfxPipeline 
{
public:

    /** @brief Create info for pipeline objects. */
    struct CreateInfo 
    {
        VkPipelineLayout                        layout;
        std::shared_ptr<RenderPass>             renderPass;               /** @brief The shaders used in this pipeline program. */
        uint32_t                                subpass;                  /** @brief What subpass this pipeline is used in. */
        std::vector<VkVertexInputBindingDescription> vertexInputBindings;
        std::vector<VkVertexInputAttributeDescription> vertexInputAttribs;
        std::vector<std::shared_ptr<GfxShader>> shaders;                  /** @brief What subpass on the render pass is this used in. */
    };

    /** @brief Constructor */
    GfxPipeline(std::shared_ptr<GfxDevice> device, const CreateInfo& info);

    /** @brief Destructor */
    ~GfxPipeline();

public:

    /** @brief Gets the raw VkPipeline underlying this object. */
    VkPipeline get() const;

    /** @brief Binds this pipeline to a command buffer. */
    void bind(VkCommandBuffer cmd);

private:
    void createPipeline(const CreateInfo& createInfo);

    std::shared_ptr<GfxDevice>  _device;
    VkPipeline                  _pipeline;
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
