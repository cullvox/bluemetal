#pragma once

#include "Device.h"
#include "Shader.h"
#include "DescriptorLayoutCache.h"
#include "RenderPass.h"

namespace bl
{

enum PipelineResourceType
{
    PIPELINE_RESOURCE_TYPE_INPUT,
    PIPELINE_RESOURCE_TYPE_OUTPUT,
    PIPELINE_RESOURCE_TYPE_SAMPLER,
    PIPELINE_RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER,
    PIPELINE_RESOURCE_TYPE_SAMPLED_IMAGE,
    PIPELINE_RESOURCE_TYPE_STORAGE_IMAGE,
    PIPELINE_RESOURCE_TYPE_UNIFORM_TEXEL_BUFFER,
    PIPELINE_RESOURCE_TYPE_STORAGE_TEXEL_BUFFER,
    PIPELINE_RESOURCE_TYPE_UNIFORM_BUFFER,
    PIPELINE_RESOURCE_TYPE_STORAGE_BUFFER,
    PIPELINE_RESOURCE_TYPE_INPUT_ATTACHMENT,
    PIPELINE_RESOURCE_TYPE_PUSH_CONSTANT_BUFFER,
};

struct PipelineMemberInfo
{
    uint32_t offset;
    uint32_t size;
    uint32_t vecSize;
    uint32_t columns;
    uint32_t arraySize;
    std::string name;
};

struct PipelineResource
{
    VkShaderStageFlags stages;
    PipelineResourceType type;
    VkAccessFlags access;
    uint32_t set;
    uint32_t binding;
    uint32_t location;
    uint32_t inputAttachmentIndex;
    uint32_t vecSize;
    uint32_t columns;
    uint32_t arraySize;
    uint32_t offset;
    uint32_t size;
    std::string name;
    std::vector<PipelineMemberInfo> members;
};


class BLUEMETAL_API Pipeline
{
public:
    Pipeline(std::shared_ptr<Device> device, std::shared_ptr<DescriptorLayoutCache> descriptorLayoutCache, std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader, std::shared_ptr<RenderPass> renderPass, uint32_t subpass = 0);
    ~Pipeline();

    VkPipelineLayout getPipelineLayout();
    VkPipeline getPipeline();
    std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts();

private:
    void getDescriptorLayouts(std::shared_ptr<DescriptorLayoutCache> descriptorLayoutCache);
    void createLayout();
    void createPipeline(std::shared_ptr<Shader> vertexShader, std::shared_ptr<Shader> fragmentShader);

    std::shared_ptr<Device>             m_device;
    std::shared_ptr<RenderPass>         m_renderPass;
    uint32_t                            m_subpass;
    std::vector<VkDescriptorSetLayout>  m_setLayouts;
    VkPipelineLayout                    m_pipelineLayout;
    VkPipeline                          m_pipeline;
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