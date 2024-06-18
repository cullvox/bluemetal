#pragma once

#include "Graphics/VulkanBlockReflection.h"
#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanDescriptorSetReflection.h"
#include "VulkanPushConstantReflection.h"
#include "VulkanDescriptorSetLayoutCache.h"
#include "VulkanPipelineLayoutCache.h"
#include "Vertex.h"

namespace bl {

/** @brief Create info for pipeline objects. */
struct VulkanPipelineStateInfo {
    
    struct Stages {
        ResourceRef<VulkanShader> vert;
        ResourceRef<VulkanShader> frag;
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

class VulkanPipelineReflection {
public:
    VulkanPipelineReflection();
    VulkanPipelineReflection(const VulkanPipelineReflection& other);
    VulkanPipelineReflection(VulkanPipelineReflection&& other);
    VulkanPipelineReflection(const VulkanPipelineStateInfo& state); /** @brief Preforms reflection/merging of reflected shader data. */
    ~VulkanPipelineReflection();

    VulkanPipelineReflection& operator=(const VulkanPipelineReflection& rhs);
    VulkanPipelineReflection& operator=(VulkanPipelineReflection&& rhs);

    std::map<uint32_t, VulkanDescriptorSetReflection>& GetDescriptorSetReflections();
    std::vector<VulkanPushConstantReflection>& GetPushConstantReflections();
    const std::map<uint32_t, VulkanDescriptorSetReflection>& GetDescriptorSetReflections() const;
    const std::vector<VulkanPushConstantReflection>& GetPushConstantReflections() const;

private:
    void ReflectMembers(VulkanBlockReflection& reflection, uint32_t binding, const SpvReflectBlockVariable& block, std::string parent = "");

    std::map<uint32_t, VulkanDescriptorSetReflection> _descriptorSetMetadata;
    std::vector<VulkanPushConstantReflection> _pushConstantMetadata;
};

/** @brief A program consisting of shaders designed to run on the GPU. */
class Pipeline {
public:
    Pipeline(VulkanDevice* device, const VulkanPipelineStateInfo& info, VkRenderPass renderPass, uint32_t subpass, const VulkanPipelineReflection* reflection = nullptr);
    ~Pipeline();

    auto GetReflection() const -> const VulkanPipelineReflection&;
    auto GetLayout() const -> VkPipelineLayout;
    auto Get() const -> VkPipeline;
    auto GetDescriptorSetLayouts() const -> const std::unordered_map<uint32_t, VkDescriptorSetLayout>&;

private:
    VulkanDevice* _device;
    VulkanPipelineReflection _reflection;
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
