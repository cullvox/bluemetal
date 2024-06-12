#pragma once

#include "Device.h"
#include "Shader.h"
#include "RenderPass.h"
#include "DescriptorSetMeta.h"
#include "PushConstantReflection.h"
#include "DescriptorSetLayoutCache.h"
#include "PipelineLayoutCache.h"
#include "Vertex.h"

namespace bl {

/** @brief Create info for pipeline objects. */
struct PipelineStateInfo {
    
    struct Stages {
        Shader* vert;
        Shader* frag;
    } stages;

    struct VertexInput {
        std::vector<VkVertexInputBindingDescription> vertexInputBindings = Vertex::GetBindingDescriptions();
        std::vector<VkVertexInputAttributeDescription> vertexInputAttribs = Vertex::GetBindingAttributeDescriptions();
    } vertexInput;

    struct InputAssembly {
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        bool primitiveRestartEnable = VK_FALSE;
    } inputAssembly;
};

class PipelineReflection {
public:
    PipelineReflection();
    PipelineReflection(const PipelineReflection& other);
    PipelineReflection(PipelineReflection&& other);
    PipelineReflection(const PipelineStateInfo& state); /** @brief Preforms reflection/merging of reflected shader data. */
    ~PipelineReflection();

    PipelineReflection& operator=(const PipelineReflection& rhs);
    PipelineReflection& operator=(PipelineReflection&& rhs);

    std::map<uint32_t, DescriptorSetReflection>& GetDescriptorSetReflections();
    std::vector<PushConstantReflection>& GetPushConstantReflections();
    const std::map<uint32_t, DescriptorSetReflection>& GetDescriptorSetReflections() const;
    const std::vector<PushConstantReflection>& GetPushConstantReflections() const;

private:
    void ReflectMembers(BlockMeta& reflection, uint32_t binding, const SpvReflectBlockVariable& block, std::string parent = "");

    std::map<uint32_t, DescriptorSetReflection> _descriptorSetMetadata;
    std::vector<PushConstantReflection> _pushConstantMetadata;
};

/** @brief A program consisting of shaders designed to run on the GPU. */
class Pipeline {
public:
    Pipeline(Device* device, RenderPass* renderPass, uint32_t subpass, const PipelineStateInfo& info, const PipelineReflection* reflection = nullptr); /** @brief Constructor */
    ~Pipeline(); /** @brief Destructor */

    const PipelineReflection& GetReflection() const;
    VkPipelineLayout GetLayout() const;
    VkPipeline Get() const; /** @brief Gets the raw VkPipeline underlying this object. */

    std::unordered_map<uint32_t, VkDescriptorSetLayout> GetDescriptorSetLayouts() const;

protected:
    Device* _device;

private:
    PipelineReflection _reflection;
    std::unordered_map<uint32_t, VkDescriptorSetLayout> _descriptorSetLayouts;
    VkPipelineLayout _layout;
    VkPipeline _pipeline;
};

} // namespace bl

/* Getting realistic about descriptor sets.

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
