#pragma once

#include "Device.h"
#include "Shader.h"
#include "RenderPass.h"
#include "DescriptorSetReflection.h"
#include "PushConstantReflection.h"
#include "DescriptorSetLayoutCache.h"
#include "PipelineLayoutCache.h"

namespace bl 
{

struct PipelineCreateInfo /** @brief Create info for pipeline objects. */
{
    RenderPass* renderPass; /** @brief What pass this pipeline is operating within. */
    uint32_t subpass; /** @brief What subpass this pipeline is used in. */
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttribs;
    std::vector<ResourceRef<Shader>> shaders; /** @brief The shaders used in this pipeline program. */
};

/** @brief A program consisting of shaders designed to run on the GPU. */
class Pipeline
{
public:
    Pipeline(Device* device, const PipelineCreateInfo& info); /** @brief Constructor */
    ~Pipeline(); /** @brief Destructor */

    VkPipelineLayout GetLayout() const;
    VkPipeline Get() const; /** @brief Gets the raw VkPipeline underlying this object. */
    std::vector<DescriptorSetReflection> GetDescriptorSetReflections() const;
    std::vector<PushConstantReflection> GetPushConstantReflections() const;

private:
    void Create(const PipelineCreateInfo& createInfo);
    void ReflectMembers(BlockReflection& reflection, const SpvReflectBlockVariable& block);

    Device* _device;
    std::vector<DescriptorSetReflection> _descriptorSetReflections;
    std::vector<PushConstantReflection> _pushConstantReflections;
    VkPipelineLayout _layout;
    VkPipeline _pipeline;
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
